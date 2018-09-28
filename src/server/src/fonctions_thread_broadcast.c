#include "fonctions_thread_broadcast.h"


/* Fonction principal du thread BROADCAST */
/******************************************/
void *broadcast(void *arg){

	struct liste_fichier *liste_file;
	struct fichier* file;
	int ligne, taille, fdClient;
	char *commande, *buffer, *fileName;

	liste_file = (struct liste_fichier *)arg;

	commande = initialiser_buffer(5);
	fileName = initialiser_buffer(50);

	while(1){
		memset(commande, 0, 5);
		memset(fileName, 0, 50);

		printf("Thread Broadcast : ATTENTE NOUVELLE COMMANDE\n");
		buffer = attente_signal_commande(liste_file, &fdClient, &ligne, &taille,
						commande, fileName);
		printf("Thread Broadcast : commande %s reçu \n", commande);

		file = trouver_Fichier(fileName);

		if(strncmp(commande, "EDITF", strlen("EDITF"))==0)
			commande_edit_BR(file, fdClient, ligne);

		else if(strncmp(commande, "SAVEF", strlen("SAVEF"))==0){
			commande_save_BR(file, fdClient, ligne, buffer, taille, 0);
			if(taille != 0)
				free(buffer);
		}

		else if(strncmp(commande, "SAVET", strlen("SAVET"))==0){
			commande_save_BR(file, fdClient, ligne, buffer, taille, 1);
			if(taille != 0)
				free(buffer);
		}

		else if(strncmp(commande, "ANNUL", strlen("ANNUL"))==0)
			commande_annul_BR(file, fdClient, ligne);

		else if(strncmp(commande, "INSRT", strlen("INSRT"))==0)
			commande_insert_BR(file, fdClient, ligne);

		else if(strncmp(commande, "DELET", strlen("DELET"))==0)
			commande_delete_BR(file, fdClient, ligne);

		else
			printf("Thread Broadcast : ERREUR Commande non connu \n");

		printf("\n\n\nListe ligne modif\n");
		display_liste(&file->listeLigneModif);
		printf("\n\n\nListe ligne\n");
		display_liste(&file->listeLigne);
	}

	free(commande);
	free(fileName);
	pthread_exit (NULL);
}


/* Insertion de ligne */
/**********************/
void commande_insert_BR(struct fichier *file, int fdClient, int num_ligne){

	int taille;
	char *envoieMess;
	struct ligneS *new_ligne;

	taille = strlen("INSRT") + sizeof(int) + 1;

	envoieMess = initialiser_buffer(taille);
	snprintf(envoieMess, taille, "INSRT%d", num_ligne);

	printf("Thread Broadcast --> envoieMess : %s\n", envoieMess);

/* Supprimer ligne de listeLigne */
	broadcast_info_All(file, envoieMess);

	new_ligne = nouvelle_ligne(num_ligne, 0, NULL);

	ajouter_ligne(file, fdClient, new_ligne);
	free(envoieMess);
}


/* Preparation du message d'une nouvelle edition a tous les utilisateurs */
/*************************************************************************/
void commande_edit_BR(struct fichier *file, int fdClient, int num_ligne){

	int taille;
	char *envoieMess;

	taille = strlen("MODIF") + sizeof(int) + 1;

	envoieMess = initialiser_buffer(taille);

	snprintf(envoieMess, taille, "MODIF%d", num_ligne);
	broadcast_info_without(file, fdClient, envoieMess);

	free(envoieMess);
}


/* Preparation du message d'une annulation d'edition a tous les utilisateurs */
/*****************************************************************************/
void commande_annul_BR(struct fichier *file, int fdClient, int num_ligne){

	int taille;
	char *envoieMess;

	taille = strlen("ANNUL") + sizeof(int) + 1;

	envoieMess = initialiser_buffer(taille);

	snprintf(envoieMess, taille, "ANNUL%d", num_ligne);

	broadcast_info_without(file, fdClient, envoieMess);

	free(envoieMess);
}


/* Preparation du message d'une suppression de ligne a tous les utilisateurs */
/*****************************************************************************/
void commande_delete_BR(struct fichier *file, int fdClient, int num_ligne){

	int taille;
	char *envoieMess;

	taille = strlen("DELET") + sizeof(int) + 1;

	envoieMess = initialiser_buffer(taille);
	snprintf(envoieMess, taille, "DELET%d", num_ligne);

	printf("Thread Broadcast --> envoieMess : %s\n", envoieMess);

	broadcast_info_All(file, envoieMess);

	suppriession_dans_deux_listes(file, fdClient, num_ligne);

//	sauvegarder_modif_save_fichier(file);

	free(envoieMess);
}


/* Preparation du message d'une sauvegarde d'edition a tous les utilisateurs */
/*****************************************************************************/
void commande_save_BR(struct fichier *file, int fdClient,
				int num_ligne, char *buffer, int taille, int tmp){

	char *buffer_envoie = NULL, *reelle_buffer = NULL;
   	int taille_envoie;

   	if(taille == 0){
   		sauvegarde_taille_zero(file, num_ligne, tmp);
  	}

  	else{

  		printf("Thread Reception --> ligne : %d taille : %d buffer : %s\n",
  						num_ligne, taille, buffer);
	   	if(tmp == 0){

			reelle_buffer = initialiser_buffer(taille + 1);

			snprintf(reelle_buffer, (taille + 1), "%s", buffer);

			redimensionner_ligne_BR(file, fdClient, num_ligne, reelle_buffer);

			free(reelle_buffer);

			//	sauvegarder_modif_save_fichier(file);
		}else{

			reelle_buffer = initialiser_buffer(taille + 1);

			snprintf(reelle_buffer, (taille + 1), "%s", buffer);

			sauvegarde_temporaire(&file->listeLigne, num_ligne, taille, reelle_buffer);

            taille++;
			taille_envoie = strlen("SAVET___") + 3*sizeof(int) + taille;

			buffer_envoie = initialiser_buffer(taille_envoie);

		    snprintf(buffer_envoie, taille_envoie, "SAVET%d_%d_%d_%s", 1,
	   	 									num_ligne, (taille - 1), reelle_buffer);

		    broadcast_info_without(file, fdClient, buffer_envoie);

		    if(reelle_buffer != NULL){
		    	free(reelle_buffer);
		    }
		    if(buffer_envoie != NULL){
				free(buffer_envoie);
			}
		}
	}
}


/* Envoie d'un message SAVE --> Modification du contenu de la ligne sans ajout */
/*******************************************************************************/
void sauvegarde_taille_zero(struct fichier *file, int num_ligne, int tmp){

	char *buffer_envoie = NULL;
   	int taille_envoie;

   	printf("sauvegarde_taille_zero --> num_ligne %d\n", num_ligne);

	if(tmp == 0){
		sauvegarder_ligne_existante(&file->listeLigneModif, &file->listeLigne,
																num_ligne, 0, NULL);

		taille_envoie = strlen("SAVEF__") + 3*sizeof(int);

		buffer_envoie = initialiser_buffer(taille_envoie);

	    snprintf(buffer_envoie, taille_envoie, "SAVEF%d_%d_%d", 1, num_ligne, 0);

	    broadcast_info_All(file, buffer_envoie);
	}else{
		sauvegarde_temporaire(&file->listeLigne, num_ligne, 0, NULL);

		taille_envoie = strlen("SAVET__") + 3*sizeof(int);

		buffer_envoie = initialiser_buffer(taille_envoie);

	   	snprintf(buffer_envoie, taille_envoie, "SAVET%d_%d_%d", 1, num_ligne, 0);
	}

	broadcast_info_All(file, buffer_envoie);

	if(buffer_envoie != NULL)
		free(buffer_envoie);
}


/* Attention !! Cette methode n'est utilisée que dans commande_save_BR et
 verrou de "listeLigneModif" est pris avant l'invocation de cette methode */
/* Redimensionner le buffer afin de créer des lignes de 80 caracteres maximum */
/******************************************************************************/
void redimensionner_ligne_BR(struct fichier *file,  int fdClient,
												int ligne, char *buffer){

	char *messEnvoie = NULL, *buffer_tmp = NULL, *buffer_final = NULL;
    char *t, *token_last, *token_previous;
    const char espace[2] = " ";
    int nb_char=0, first=0, taille = 0, compteur = 0;

    messEnvoie = initialiser_buffer(79);
    buffer_final = initialiser_buffer(79);

/* On enleve tous les " " du buffer et création de buffer de 80 carateres maximum */
    t = strtok(buffer, espace);
    token_last = strdup(t);
    t = strtok(NULL, espace);

    while(t != NULL){
        token_previous = strdup(token_last);
        token_last = strdup(t);

        taille = (int)strlen(token_previous);
        taille += 2;	/* 2 --> " " + "\0" */

        buffer_tmp = initialiser_buffer(taille);

	    snprintf(buffer_tmp,  taille, "%s ", token_previous);

        if((nb_char + taille) > 79){
    	/* 1e ligne est presente dans LigneModif mais on doit créer les autres */
        	/* Donc first = 1 puis on incrément first pour dire qu'il faut créer
        			les lignes suivantes a ajouter  */
        	/* first sert aussi de compteur */

        	preparation_broadcast_save(file, fdClient, ligne+first, (int)strlen(messEnvoie),
        												messEnvoie, first);
        	first++;

            memset(messEnvoie, 0, 79);
            nb_char = 0;
        }

        nb_char += (taille - 1);

        if(compteur == 0){
        	snprintf(messEnvoie, taille, "%s ", buffer_tmp);
        	nb_char = strlen(messEnvoie);

       	}else{
       		snprintf(buffer_final, (nb_char + 1), "%s%s", messEnvoie, buffer_tmp);
    		memset(messEnvoie, 0, 79);
    		snprintf(messEnvoie, (nb_char + 1), "%s", buffer_final);

       		memset(buffer_final, 0, 79);
    		nb_char = strlen(messEnvoie);
       	}

       	compteur++;
       	taille = 0;

        t = strtok(NULL, espace);
     	if(buffer_tmp != NULL){
     		free(buffer_tmp);
			buffer_tmp = NULL;
     	}
    }

    taille += strlen(token_last);
    taille += 1;

    if(nb_char + taille > 79){

    	printf("Thread Broadcast --> HEEELP ***********messEnvoie : '%s'  taille : %d\n",
	    												messEnvoie, nb_char);

    	preparation_broadcast_save(file, fdClient, ligne+first, strlen(messEnvoie),
    												messEnvoie, first);
    	first++;

        nb_char = 0;

        nb_char += taille;
	    memset(messEnvoie, 0, 79);
	   	snprintf(messEnvoie, (nb_char + 1), "%s", token_last);

	   	printf("Thread Broadcast --> ICIIIIIIIIIIIIIIII messEnvoie : '%s'  taille : %d\n",
		    											messEnvoie, (nb_char - 1));

	    preparation_broadcast_save(file, fdClient, ligne+first, nb_char,
	    												messEnvoie, first);

    }else{

	    nb_char += taille;

		memset(buffer_final, 0, 79);
	    snprintf(buffer_final, strlen(messEnvoie) + strlen(token_last) + 1, "%s%s",
	    											messEnvoie, token_last);
	    memset(messEnvoie, 0, 79);
	   	snprintf(messEnvoie, nb_char + 1, "%s", buffer_final);

	   	printf("Thread Broadcast --> LAAAAAAAAAAAAAAAAA messEnvoie : '%s'  taille : %d\n",
		    								messEnvoie, nb_char);

	    preparation_broadcast_save(file, fdClient, ligne+first, strlen(messEnvoie),
	    												messEnvoie, first);
	    first++;
	}

	if(messEnvoie != NULL)
    	free(messEnvoie);

    if(buffer_final != NULL)
   		free(buffer_final);
}


/* Attention !! Cette methode n'est utilisée que dans redimensionner_ligne_BR
 	et le verrou de "listeLigneModif" est pris avant l'invocation de la
 	methode redimensionner_ligne_BR */
/* Création et ajout de ligne dans la liste et complétion du message d'envoie */
/******************************************************************************/
void preparation_broadcast_save(struct fichier *file,  int fdClient, int num_ligne,
										int tailleBuf, char *buffer, int first){
	int taille;
	char *envoieMess;
	struct ligneS *new;

	taille = strlen("SAVEF___") + 3*sizeof(int) + tailleBuf;

	envoieMess = initialiser_buffer(taille);

	snprintf(envoieMess, taille, "SAVEF%d_%d_%d_%s", first, num_ligne,
															tailleBuf, buffer);

/* First sert a dissocier les lignes qui sont créeées en plus de celle que
	l'utilisateur était entrain d'éditer --> depassement de 80 caracteres */
	if(first == 0){

		printf("FIRSSSSSSSSSSSSSSSSSSSSSSST ligne : %d\n", num_ligne);
		sauvegarder_ligne_existante(&file->listeLigneModif, &file->listeLigne,
													num_ligne, tailleBuf, buffer);

	}else{
		printf("OTHEEEEEEEEEEEEEEEEEEEEEEEEEER \n");
		new = nouvelle_ligne(num_ligne, tailleBuf, buffer);
    	ajouter_ligne(file, fdClient, new);
	}

	broadcast_info_All(file, envoieMess);
	free(envoieMess);
}


/* À Utiliser pour les Commandes EDITF et ANNUL */
/* Envoie d'une MàJ à tous les utilisateurs sauf à celui qui l'a déclenché */
/***************************************************************************/
void broadcast_info_without(struct fichier *file, int fdClient, char *envoieMess){

	struct client *cible;

	pthread_mutex_lock(&file->mutexTabFdSock);
	printf("Thread Broadcast Without --> envoie du message : '%s'\n", envoieMess);

	cible = file->fdSocks.next;

	while(cible != &file->fdSocks){
		if((cible->fdSockEmission != fdClient) &&
			(cible->fdSockReception != fdClient) &&
			(cible->client_deconnecte == 0)){
			if(write(cible->fdSockEmission, envoieMess, strlen(envoieMess) + 1) == -1){
				printf("***********************Thread Broadcast : Le client est tombé en panne \n");
				cible->client_deconnecte = 1;
				cible->client_en_panne = 0;
			}
		}
		cible = cible->next;
	}
	pthread_mutex_unlock(&file->mutexTabFdSock);
}


/* À Utiliser pour les Commandes SAVEF et DELET */
/* Envoie d'une mise à jour à tous les utilisateurs */
/****************************************************/
void broadcast_info_All(struct fichier *file, char *envoieMess){

	struct client *cible;

	pthread_mutex_lock(&file->mutexTabFdSock);
	printf("Thread Broadcast All --> envoie du message : '%s'\n", envoieMess);

	cible = file->fdSocks.next;

	while(cible != &file->fdSocks){;
		if(cible->client_deconnecte == 0){
			if(write(cible->fdSockEmission, envoieMess,	strlen(envoieMess) + 1) == -1){
				printf("********************Thread Broadcast : Le client est tombé en panne \n");
				cible->client_deconnecte = 1;
				cible->client_en_panne = 0;
			}
		}
		cible = cible->next;
	}
	pthread_mutex_unlock(&file->mutexTabFdSock);
}


/* On remplie la struct commande du thread Broadcast pour lui signaler une commande */
/************************************************************************************/
char *attente_signal_commande(struct liste_fichier *file, int *fdClient, int *ligne,
									int *taille, char *commande, char *fileName){
	char *buffer = NULL;

	printf("Thread Broadcast : Attente de commande\n");

	pthread_mutex_lock(&file->struct_br.mutexCommandeBR);
	if(file->struct_br.commandeBR.rempli == 0){
		pthread_cond_wait(&file->struct_br.condCommandeBR,
								&file->struct_br.mutexCommandeBR);
	}

	strcpy(fileName, file->struct_br.fileName);
	strcpy(commande, file->struct_br.commandeBR.commandeName);

	*ligne = file->struct_br.commandeBR.ligne.num;
	*taille = file->struct_br.commandeBR.ligne.taille;
	*fdClient = file->struct_br.fd_client;

	if(file->struct_br.commandeBR.ligne.buffer != NULL){

		buffer = initialiser_buffer(*taille);
		strncpy(buffer, file->struct_br.commandeBR.ligne.buffer, *taille);

		free(file->struct_br.commandeBR.ligne.buffer);
	}

	free(file->struct_br.fileName);
/* Je reveille le thread broadcast */
	file->struct_br.commandeBR.rempli=0;
	pthread_cond_signal(&file->struct_br.condCommandeBR);
	pthread_mutex_unlock(&file->struct_br.mutexCommandeBR);

	return buffer;
}


/* Fonction utiliser par le thread Emission et Reception
pour signaler une nouvelle commande au thread BROADCASt */
/********************************************************/
void signaler_thread_broadcast(struct liste_fichier *file, int fd_client, char *type,
							int ligne, int taille, char *buffer, char *fileName){
	int taille_fileName, reel_size, size_buf;

	if(taille != 0){
		size_buf = strlen(buffer);
	}else{
		size_buf = 0;
	}

	if(taille < size_buf){
		reel_size = taille;
	}else{
		reel_size = size_buf;
	}

/* Je vérifie que le thread broadcast a traité lancienne commande */
	pthread_mutex_lock(&file->struct_br.mutexCommandeBR);
	if(file->struct_br.commandeBR.rempli == 1){
		pthread_cond_wait(&file->struct_br.condCommandeBR,
											&file->struct_br.mutexCommandeBR);
	}

/* REMPLIR LES CHAMPS DE LA STRUCTURE */
	taille_fileName = strlen(fileName);
	file->struct_br.fileName = initialiser_buffer(taille_fileName);

	strcpy(file->struct_br.fileName, fileName);
	strcpy(file->struct_br.commandeBR.commandeName, type);
	file->struct_br.commandeBR.ligne.num = ligne;
	file->struct_br.commandeBR.ligne.taille = reel_size;

	if(buffer != NULL){

		file->struct_br.commandeBR.ligne.buffer = initialiser_buffer(reel_size + 1);
		snprintf(file->struct_br.commandeBR.ligne.buffer,  reel_size + 1, "%s",buffer);

		printf("Signale eu Thread BR --> num : %d taille : %d buffer : '%s'\n",
					ligne, reel_size, file->struct_br.commandeBR.ligne.buffer);
	}else{
		file->struct_br.commandeBR.ligne.buffer = NULL;
	}

	file->struct_br.fd_client = fd_client;

/* Je reveille le thread broadcast */
	file->struct_br.commandeBR.rempli=1;
	pthread_cond_signal(&file->struct_br.condCommandeBR);
	pthread_mutex_unlock(&file->struct_br.mutexCommandeBR);
}
