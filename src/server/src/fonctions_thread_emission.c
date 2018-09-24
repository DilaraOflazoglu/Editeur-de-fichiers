#include "fonctions_thread_emission.h"


//OK
/* Fonction principal du thread EMISSION */
/*****************************************/
void *emission(void *arg){

	char *fileName;
	int ligne, taille;
	struct client *clt;
	struct fichier *file = NULL;

	clt = (struct client *)arg;

	envoie_noms_fichier(clt);

	clt->commandeName_Em = initialiser_buffer(6);

	while(1){
		memset(clt->commandeName_Em, 0, 5);
		printf("Thread Emission : Attente de REQUETE !\n");

		clt->buffer_Em = attente_reception_commande(clt, &ligne, &taille,
															clt->commandeName_Em);

		printf("Thread Emission : commande '%s' reçu \n", clt->commandeName_Em);

		if(strncmp(clt->commandeName_Em, "OPENF", strlen("OPENF"))==0){

			fileName = initialiser_buffer(taille);

			snprintf(fileName, taille + 1, "%s", clt->fileName);

			printf("Thread Emission 1 --> fileName : %s\n", fileName);
			file = trouver_Fichier(fileName);

			envoie_fichier(file, clt);

			free(fileName);
		}

		else if(strncmp(clt->commandeName_Em, "EDITF", strlen("EDITF")) == 0)
			commande_edit_Em(file, clt, ligne);


		else if(strncmp(clt->commandeName_Em, "ANNUL", strlen("ANNUL")) == 0)
			commande_annul_Em(file, clt,ligne);


		else if(strncmp(clt->commandeName_Em, "CLOSE", strlen("CLOSE")) == 0){
			commande_close_Em(file, clt);
			break;
		}

		else
			printf("Thread Emission : ERREUR Commande non connu \n");


		free(clt->buffer_Em);
		clt->buffer_Em = NULL;
	}

	free(clt->commandeName_Em);
	clt->commandeName_Em = NULL;
	printf("***********************Thread Emission : TERMINAISON\n");
	pthread_exit (NULL);
}


//OK
/* Annulation d'édition par un utilisateur signalé au thread Broadcast */
/***********************************************************************/
void commande_annul_Em(struct fichier *file, struct client *clt, int ligne){

	int taille = 0;
	struct ligneS *cible;

	pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);
	cible = file->listeLigneModif.tabLigne.next;

	while(cible !=  &file->listeLigneModif.tabLigne){
		if(cible->num == ligne){
			supprimer_ligne(cible);
			free(cible);
			break;
		}
		cible = cible->next;
	}
	pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);



/* Dans listeModif il n'y que des lignes et leurs numeros de ligne sans buffer */
	signaler_thread_broadcast(&liste_file, clt->fdSockEmission, "ANNUL",
												ligne, 0, NULL, clt->fileName);

	if(clt->cache != NULL){
		taille = strlen(clt->cache);
	}

/* Pour remettre la ligne modifié à son état initiale */
	signaler_thread_broadcast(&liste_file, clt->fdSockReception, "SAVEF",
									ligne, taille, clt->cache, clt->fileName);
	if(clt->cache != NULL){
		free(clt->cache);
		clt->cache = NULL;
	}
}


//OK
/* Nouvelle Edition par une utilisateur signalé au thread Broadcast */
/********************************************************************/
void commande_edit_Em(struct fichier *file, struct client *clt, int ligne){

	int taille;
	char *envoie;

/* Si liste ligne modif contient le numero ligne */
	if(contains_ligne(&file->listeLigneModif, ligne) == 1){

		if(write(clt->fdSockEmission, "EDITFKO", strlen("EDITFKO")) == -1){
			client_tombe_en_panne(file, clt);
			return;
		}
	}

	else{

		taille = strlen("EDITFOK") + sizeof(int);
		envoie = initialiser_buffer(taille);
		snprintf(envoie, taille,"EDITFOK%d", ligne);
		remplir_cache(&file->listeLigne, clt, ligne);

		pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);

		nouvelle_ligne_vide(&file->listeLigneModif, ligne);

		taille = strlen(envoie) + 1;
		printf("Thread Emission : Reponse EDITF '%s' taille : %d\n", envoie, taille);

		if(write(clt->fdSockEmission, envoie, taille) == -1){
			client_tombe_en_panne(file, clt);
			pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);
			free(envoie);
			return;
		}

		pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);

		pthread_mutex_lock(&file->mutexTabFdSock);
		clt->en_cours_dedition = ligne;
		pthread_mutex_unlock(&file->mutexTabFdSock);

		signaler_thread_broadcast(&liste_file, clt->fdSockEmission, "EDITF",
												ligne, 0, NULL, clt->fileName);
		free(envoie);
	}

	display_liste(&file->listeLigneModif);
}


//OK
/* Fermeture de l'application pas l'utilisateur apres ouverture d'un fichier */
/*****************************************************************************/
void commande_close_Em(struct fichier *file, struct client *clt){

/* Le client a choisi un fichier donc, suppression du client de
	la liste de client du fichier */
	if(file != NULL){
		pthread_mutex_lock(&file->mutexTabFdSock);
		if(write(clt->fdSockEmission, "CLOSE", strlen("CLOSE")) < 0){
			perror("Socket Client cassé \n");
		}

	/* On dit au thread check que se client n'existe plus */
		clt->client_deconnecte = 1;
		clt->client_en_panne = 0;
		pthread_mutex_unlock(&file->mutexTabFdSock);


	}

/* Le client n'a s encore choisi de fichier donc, il est la liste
	temporaire de client, donc on le supprime de cette liste */
	else{

		pthread_mutex_lock(&liste_file.mutex);
		if(write(clt->fdSockEmission, "CLOSE", strlen("CLOSE")) < 0){
			perror("Socket Client cassé \n");
		}

	/* On dit au thread check que se client n'existe plus */
		clt->client_deconnecte = 1;
		clt->client_en_panne = 0;
		pthread_mutex_unlock(&liste_file.mutex);
	}
}


//OK
/* Attendre d'être signaler d'une commande au thread recetion */
/**************************************************************/
char *attente_reception_commande(struct client *clt, int *ligne,
											int *taille, char *commande){
	char *buffer = NULL;
	printf("Thread Emission : Attente de commande\n");

/* J'attends la recepton d'une commande */
	pthread_mutex_lock(&clt->mutexCommande);

	if(clt->commandeInfo.rempli == 0){
		pthread_cond_wait(&clt->condCommande, &clt->mutexCommande);
	}

	strcpy(commande, clt->commandeInfo.commandeName);

	*ligne = clt->commandeInfo.ligne.num;
	*taille = clt->commandeInfo.ligne.taille;

	if(clt->commandeInfo.ligne.buffer != NULL){

		buffer = initialiser_buffer(*taille);
		strcpy(buffer, clt->commandeInfo.ligne.buffer);

		free(clt->commandeInfo.ligne.buffer);
	}

	clt->commandeInfo.rempli=0;
	pthread_cond_signal(&clt->condCommande);
	pthread_mutex_unlock(&clt->mutexCommande);

	return buffer;
}


//OK
/* Envoie de tous les noms de fichiers existant sur le serveur */
/***************************************************************/
void envoie_noms_fichier(struct client *cible){

	int taille = 0;

	envoie_nom_dun_fichier(cible, "histoire.txt");

	envoie_nom_dun_fichier(cible, "data_privacy.txt");

	envoie_nom_dun_fichier(cible, "technology.txt");

	if(write(cible->fdSockEmission, &taille, sizeof(int)) == -1){
		printf("*********************Thread Emission : Le client est tombé en panne \n");
		pthread_mutex_lock(&liste_file.mutex);
		cible->client_deconnecte = 1;
		cible->client_en_panne = 0;
		pthread_mutex_unlock(&liste_file.mutex);
		return;
	}
}


void envoie_nom_dun_fichier(struct client *cible, char *name){

	char *fileN;
	int taille;

	taille = strlen(name) + 1;
	fileN = initialiser_buffer(taille);

	memset(fileN, 0, taille);
	snprintf(fileN, taille, "%s", name);

	if(write(cible->fdSockEmission, &taille, sizeof(int)) == -1){
		printf("***************Thread Emission : Le client est tombé en panne \n");
		pthread_mutex_lock(&liste_file.mutex);
		cible->client_deconnecte = 1;
		cible->client_en_panne = 0;
		pthread_mutex_unlock(&liste_file.mutex);
		free(fileN);
		return;
	}

/*  Envoie du nom du fichier */
	if(write(cible->fdSockEmission, fileN, taille) == -1){
		printf("****************Thread Emission : Le client est tombé en panne \n");
		pthread_mutex_lock(&liste_file.mutex);
		cible->client_deconnecte = 1;
		cible->client_en_panne = 0;
		pthread_mutex_unlock(&liste_file.mutex);
		free(fileN);
		return;
	}

	free(fileN);
}


//OK
/* Envoie du contenu du fichier a cet instant T */
/************************************************/
void envoie_fichier(struct fichier *file, struct client *clt){

	struct ligneS *cible, *cible_modif;
	int num_ligne = 0;

	if(write(clt->fdSockEmission, "OPENF", strlen("OPENF")) == -1){
		client_tombe_en_panne(file, clt);
		return;
	}

	pthread_mutex_lock(&file->listeLigne.mutexListeLigne);

	cible = file->listeLigne.tabLigne.next;
	while(cible != &file->listeLigne.tabLigne){

		if(write(clt->fdSockEmission, &cible->num, sizeof(int)) == -1){
			client_tombe_en_panne(file, clt);
			pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
			return;
		}

        if(write(clt->fdSockEmission, &cible->taille, sizeof(int)) == -1){
        	client_tombe_en_panne(file, clt);
			pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
			return;
        }

        if(cible->buffer != NULL){

	        if(write(clt->fdSockEmission, cible->buffer, cible->taille) == -1){
	        	client_tombe_en_panne(file, clt);
				pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
				return;
	        }

	       printf("**************** num_ligne : %d taille : %d buffer : %s\n",
        						cible->num, cible->taille, cible->buffer);
	    }else{
	    	if(write(clt->fdSockEmission, "", 0) == -1){
	    		client_tombe_en_panne(file, clt);
				pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
				return;
	    	}
	    }

		cible = cible->next;
	}
	pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);

	if(write(clt->fdSockEmission, &num_ligne, sizeof(int)) == -1){
		client_tombe_en_panne(file, clt);
		return;
	}
	printf("Fichier envoyer ! \n");

	pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);
	cible_modif = file->listeLigneModif.tabLigne.next;

	while(cible_modif !=  &file->listeLigneModif.tabLigne){

		printf("¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨cible_modif->num : %d\n", cible_modif->num);
	 	if(write(clt->fdSockEmission, &cible_modif->num, sizeof(int)) == -1){
	 		client_tombe_en_panne(file, clt);
			pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);
			return;
	 	}

		cible_modif = cible_modif->next;
	}
	pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);

	if(write(clt->fdSockEmission, &num_ligne, sizeof(int)) == -1){
		client_tombe_en_panne(file, clt);
		return;
	}

	printf("Liste ligne modif envoyer ! \n");
}


/* Le Thread Emission repere que le client ne reçoit plus rien via la socket */
/* 	Reperage d'une panne du coté du Client, signalisation au thread check */
/*****************************************************************************/
void client_tombe_en_panne(struct fichier *file, struct client *clt){
	printf("*******************Thread Emission : Le client est tombé en panne \n");
	pthread_mutex_lock(&file->mutexTabFdSock);
	clt->client_deconnecte = 1;
	clt->client_en_panne = 0;
	pthread_mutex_unlock(&file->mutexTabFdSock);
}