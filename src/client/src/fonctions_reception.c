#include "fonctions_reception.h"


/* Fonction principal du thread RECEPTEUR */
/******************************************/
void *reception_reponse(void *arg){

    int fdSockClientReception;
	char *Cname;

	fdSockClientReception = (*((int *)arg));

	Cname = initialiser_buffer(5);

	reception_nom_fichiers(fdSockClientReception);

	while(read(fdSockClientReception, Cname, 5) > 0){
		printf("Thread recepteur : Commande '%s' reçu ! \n", Cname);

/* Reception d'un ouverture de fichier par l'utilisateur */
		if(strncmp(Cname, "OPENF", strlen("OPENF")) == 0){
			reception_fichier(fdSockClientReception);


/* Reception de permission ou non de modifier une ligne */
		}else if(strncmp(Cname, "EDITF", strlen("EDITF")) == 0){
			reception_edit(fichierInfo, fdSockClientReception);


/* reception d'un insertion de ligne --> lors d'un '\n' */
		}else if(strncmp(Cname, "INSRT", strlen("INSRT")) == 0){
			reception_insert(fichierInfo, fdSockClientReception);


		/* Reception d'un sauvegarde d'un autre autre utilisateur */
        }else if(strncmp(Cname, "SAVET", strlen("SAVET")) == 0){
        	reception_broadcast(fichierInfo, fdSockClientReception, 1);

/* Reception d'un sauvegarde d'un autre autre utilisateur */
        }else if(strncmp(Cname, "SAVEF", strlen("SAVEF")) == 0){
        	reception_broadcast(fichierInfo, fdSockClientReception, 0);


/* Reception d'une nouvelle edition d'un autre autre utilisateur */
    	}else if(strncmp(Cname, "MODIF", strlen("MODIF")) == 0){
    		reception_modif(fichierInfo, fdSockClientReception);

/* Reception d'une suppression de ligne d'un autre utilisateur */
    	}else if(strncmp(Cname, "DELET", strlen("DELET")) == 0){
    		reception_delete(&fichierInfo->listeLigneModif,
    						&fichierInfo->listeLigne, fdSockClientReception);

/* Reception d'une annulation d'édition de ligne d'un autre utilisateur */
    	}else if(strncmp(Cname, "ANNUL", strlen("ANNUL")) == 0){
    		reception_cancel(&fichierInfo->listeLigneModif, fdSockClientReception);


/* Reception de la fermeture du fichier par l'utilisateur */
    	}else if(strncmp(Cname, "CLOSE", strlen("CLOSE")) == 0){
			break;

		}else{
			printf("Thread recepteur : ERREUR commande non inconnu ! \n");
		}
		memset(Cname, 0, 5);
	}

	free(Cname);
	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->attente_de_reponse = 0;
	fichierInfo->nb_non_reponse = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	fermeture_socket_tcp(fdSockClientReception);
	printf("Thread recepteur : Terminaison \n");
	pthread_exit (NULL);
}


/* Insertion de ligne */
/**********************/
void reception_insert(struct fichier *file, int fdSockServer){

	struct ligneC *new_ligne;
	int ligne;

	ligne = reception_entier(fdSockServer);

	if((new_ligne = (struct ligneC *)malloc(sizeof(struct ligneC))) == NULL){
		perror("Création LigneS : PB malloc LigneS !"); exit(-1);
	}

	new_ligne->buffer = NULL;
	new_ligne->num = ligne;
	new_ligne->taille = 0;
	new_ligne->next = new_ligne;
	new_ligne->prev = new_ligne;

	pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);
	pthread_mutex_lock(&file->listeLigne.mutexListeLigne);
	ajouter_ligne(&file->listeLigneModif, &file->listeLigne, new_ligne);
	pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
	pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	if(fichierInfo->updated == 0){
		fichierInfo->updated = 1;
	}
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
}


/* Reception de la réponse de la demande d'edition faite par l'utilisateur */
/***************************************************************************/
void reception_edit(struct fichier *file, int fdSockClientReception){

	char *reponse_edit;
	int ligne;

	reponse_edit = initialiser_buffer(3);

	memset(reponse_edit, 0, 2);
	if(read(fdSockClientReception, reponse_edit, 2)==-1){
		perror("Thread Reception : Probleme read reponse_edit EDITF");
		goto erreur_read_reponse_edit;
	}

/* Verif permission d'éditer */
	if(strcmp(reponse_edit, "OK")==0){

		ligne = reception_entier(fdSockClientReception);

		pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
		fichierInfo->en_cours_dedition = ligne;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

		nouvelle_ligne_vide(&file->listeLigneModif, ligne);
		printf("Thread Reception : Autorisation reçu pour Editer\n");

	}else if(strcmp(reponse_edit, "KO") ==0){
		printf("Thread Reception : Pas d'autorisation Editer\n");

		pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
		fichierInfo->en_cours_dedition = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

	}else{
		printf("Thread Reception : Réponse pour EDITF non reconnu !\n");
	}

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->attente_de_reponse = 0;
	fichierInfo->nb_non_reponse = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

	free(reponse_edit);
	return;


erreur_read_reponse_edit :
	free(reponse_edit);
	exit(-1);
}


/* Reception d'une annulation d'edtiion d'un Utilsateur */
/********************************************************/
void reception_cancel(struct liste_ligneC *listeModif, int fdSockClientReception){

	int num_ligne;
	struct ligneC *cible;

	num_ligne = reception_entier(fdSockClientReception);
	pthread_mutex_lock(&listeModif->mutexListeLigne);
	cible = listeModif->tabLigne.next;

	while(cible != &listeModif->tabLigne){
		if(cible->num == num_ligne){
			supprimer_ligne(cible);
			pthread_mutex_unlock(&listeModif->mutexListeLigne);
			detruire(cible);
			return;

		}
		cible = cible->next;
	}
	pthread_mutex_unlock(&listeModif->mutexListeLigne);
}


/* Reception d'une suppression de ligne d'un Utilisateur */
/*********************************************************/
void reception_delete(struct liste_ligneC *listemodif,
					struct liste_ligneC *liste, int fdSockClientReception){

	int num_ligne;

	num_ligne = reception_entier(fdSockClientReception);

	suppriession_dans_deux_listes(listemodif,liste, num_ligne);

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	if(fichierInfo->updated == 0){
		fichierInfo->updated = 1;
	}

	if((fichierInfo->en_cours_dedition == num_ligne) &&
		(fichierInfo->attente_de_reponse == 1)){
		fichierInfo->en_cours_dedition = 0;
		fichierInfo->attente_de_reponse = 0;
		fichierInfo->nb_non_reponse = 0;
	}
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
}


/* Reception d'edition de ligne d'un autre User */
/************************************************/
void reception_modif(struct fichier *file, int fdSockClientReception){

	int ligne;

	ligne = reception_entier(fdSockClientReception);
	nouvelle_ligne_vide(&file->listeLigneModif, ligne);
}


/* Chaque  envoie d'entier par le serveur se termine  par un underscore (_) */
/* Cette Fonction permet d'extraire l'entier du message reçu par le Serveur */
/****************************************************************************/
int reception_entier(int fdSockClientReception){

	char *carac, *tmp;
	int entier;

	carac = initialiser_buffer(3);

	tmp = initialiser_buffer(10);

	while(read(fdSockClientReception, carac, sizeof(char))){
		if((strcmp(carac, "_") == 0) || (strcmp(carac, "\0") == 0))
			break;

		strncat(tmp, carac, 1);
		memset(carac, 0, 3);
	}

	entier = atoi(tmp);
	free(carac);
	free(tmp);
	return entier;
}


/* Reception d'une sauvegarde d'une ligne */
/******************************************/
void reception_broadcast(struct fichier *file, int fdSockClientReception, int tmp){

	char *buffer, *buf;
	struct ligneC *new;
	int taille, num_ligne, first;

	first = reception_entier(fdSockClientReception);
	num_ligne = reception_entier(fdSockClientReception);
	taille = reception_entier(fdSockClientReception);

	if(tmp == 0){

		if(taille == 0){

			sauvegarder_ligne_existante(&file->listeLigneModif, &file->listeLigne,
												num_ligne, 0, NULL);
			printf("Thread Reception --> first : %d num_ligne : %d taille : %d \n",
												first, num_ligne, taille);
		}else{

		/* Si fin du message faire ++ car caractere '\0' */
			taille++;

			buf = initialiser_buffer(taille);

			if(read(fdSockClientReception, buf, taille) == -1){
				perror("Thread Reception : Probleme read buffer BCAST");
				goto error_out_2;
			}

			buffer = initialiser_buffer(taille);
			snprintf(buffer, taille, "%s", buf);

			printf("Thread Reception --> first : %d num_ligne : %d taille : %d  buffer : '%s'\n",
												first, num_ligne, taille, buffer);

			if(first == 0){

				sauvegarder_ligne_existante(&file->listeLigneModif, &file->listeLigne,
														num_ligne, taille, buffer);
				printf("FIRSSSSSSSSSSSSSSSSSSSSSSST ligne : %d\n", num_ligne);
			}else{
				new = nouvelle_ligne(num_ligne, taille, buffer);
				pthread_mutex_lock(&file->listeLigne.mutexListeLigne);
		    	ajouter_ligne(&file->listeLigneModif, &file->listeLigne, new);
		    	pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
			}
		  	free(buf);
			free(buffer);
		}

		pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
		if(fichierInfo->updated == 0){
				fichierInfo->updated = 1;
			}

		if(fichierInfo->en_cours_dedition == num_ligne &&
		(fichierInfo->attente_de_reponse == 1)){
			fichierInfo->en_cours_dedition = 0;
			fichierInfo->attente_de_reponse = 0;
		}
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
		return;
	}

	else{

		if(taille == 0){
			buffer = NULL;
			sauvegarde_temporaire(&file->listeLigne, num_ligne, taille, buffer);
		}else{

			taille ++;
			buf = initialiser_buffer(taille);

			if(read(fdSockClientReception, buf, taille) ==-1){
				perror("Thread Reception : Probleme read buffer BCAST");
				goto error_out_2;
			}

			buffer = initialiser_buffer(taille);
			snprintf(buffer, taille, "%s", buf);

			printf("Thread Reception --> first : %d num_ligne : %d taille : %d  buffer : '%s'\n",
											first, num_ligne, (taille - 1), buffer);
			sauvegarde_temporaire(&file->listeLigne, num_ligne, (taille - 1), buffer);

			pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
			if(fichierInfo->updated == 0){
				fichierInfo->updated = 1;
			}

			if(fichierInfo->en_cours_dedition == num_ligne
				&& (fichierInfo->attente_de_reponse == 1)){
				fichierInfo->nb_non_reponse = 0;
			}
			pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

			free(buf);
			free(buffer);
		}
	}
	printf("---------------- Fin SAVE --------------\n");
	return;

error_out_2 :
	free(buf);
	exit(-1);
}


/* Reception des noms de fichiers pour que l'user pense les avoir localement */
/*****************************************************************************/
void reception_nom_fichiers(int fdServer){

	int fdFile, taille, taille_rep;
	char *fName, *repertoire;

	printf("Thread recepteur : reception des noms des fichiers \n");

	mkdir("./fichier", 0777);

	while(1){
	/* Reception de la taille du nom du fichier  */
		if(read(fdServer, &taille, sizeof(int)) == -1){
			perror("Thread Reception : Probleme read taille");
			goto error_out_1;
		}

	/* Condition d'arrêt */
        if(taille == 0) break;

		fName = initialiser_buffer(taille);
	/* Reception du nom du fichier*/
		if(read(fdServer, fName, taille)==-1){
			perror("Thread Reception : Probleme read fName");
			goto error_out_2;
		}

		taille_rep = strlen("fichier/") + taille;
		repertoire = initialiser_buffer(taille_rep);

		strcat(repertoire, "fichier/");
		strcat(repertoire, fName);

		if((fdFile = open(repertoire, O_CREAT | O_RDWR, 0777))==-1){
			perror("Thread Reception : Probleme open");
			goto erreur_open_file;
		}

		close(fdFile);
		free(fName);
		free(repertoire);
	}

	return;

erreur_open_file :
	free(repertoire);

error_out_2 :
	free(fName);

error_out_1 :
	exit(-1);
}


/* Reception du fichier et création de la liste de ligne */
/*********************************************************/
void reception_fichier(int fdSockServer){

    int taille, num_ligne = 0, ligne_modif = 0;
   	struct ligneC *new;
    char *messRecu;

    printf("Thread recepteur : reception du fichier\n");

    pthread_mutex_lock(&fichierInfo->mutexfileName);
	if(fichierInfo->fileName_rempli == 0){
		pthread_cond_wait(&fichierInfo->condfileName, &fichierInfo->mutexfileName);
	}
	pthread_mutex_unlock(&fichierInfo->mutexfileName);

/* Lecture dans la socket pour ecrire dans le fichier */
    while(read(fdSockServer, &num_ligne, sizeof(int)) != -1){

    /* Condition d'arrêt */
        if(num_ligne == 0)
        	break;

        if(read(fdSockServer, &taille, sizeof(int))==-1){
        	perror("Thread Reception : Probleme read taille");
        	goto error_out_2;
        }

        messRecu = initialiser_buffer(taille);

        if(read(fdSockServer, messRecu, taille)==-1){
        	perror("Thread Reception : Probleme read messRecu");
        	goto error_out_2;
        }

        printf("**************** num_ligne : %d taille : %d buffer : %s\n",
        						num_ligne, taille, messRecu);

        new = nouvelle_ligne(num_ligne, taille, messRecu);
        pthread_mutex_lock(&fichierInfo->listeLigne.mutexListeLigne);
        ajouter_ligne(&fichierInfo->listeLigneModif, &fichierInfo->listeLigne, new);
        pthread_mutex_unlock(&fichierInfo->listeLigne.mutexListeLigne);
        num_ligne++;
        free(messRecu);
    }

    printf("Fichier recu ! \n");

    while(read(fdSockServer, &ligne_modif, sizeof(int)) != -1){
    /* Condition d'arrêt */
        if(ligne_modif == 0) break;

        nouvelle_ligne_vide(&fichierInfo->listeLigneModif, ligne_modif);
    }

    printf("Liste ligne modif recu ! \n");

/* Signaler au thread emetteur que le fichier est rempli */
	pthread_mutex_lock(&fichierInfo->mutexfileName);
	fichierInfo->fileName_rempli = 0;
	pthread_cond_signal(&fichierInfo->condfileName);
	pthread_mutex_unlock(&fichierInfo->mutexfileName);

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->attente_de_reponse = 0;
	fichierInfo->nb_non_reponse = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

    return;


error_out_2 :
	free(messRecu);
	exit(-1);
}
