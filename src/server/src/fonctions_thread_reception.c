
#include "fonctions_thread_reception.h"


//OK
/* Fonction principal du thread RECEPTION */
/******************************************/
void *reception(void *arg){

	struct fichier *file=NULL;
	int nb_commande_non_reconnu = 0;
	struct client *clt = (struct client *)arg;

	clt->commandeName_Recep = initialiser_buffer(5);

	while(1){
		memset(clt->commandeName_Recep, 0, 5);

		printf("Thread recepteur : Attente de REQUETE !\n");

		if(read(clt->fdSockReception, clt->commandeName_Recep , 5)==-1){
		    perror("Thread recepteur : Probleme read commande"); exit(-1);
		}

		printf("Thread recepteur : Commande '%s' reçu ! \n", clt->commandeName_Recep);

	/* Demande téléchargement fichier */
		//OPENF-SIZE-FILENAME
		if(strncmp(clt->commandeName_Recep, "OPENF", strlen("OPENF"))==0){
			file = commande_open_RECEP(clt);
			nb_commande_non_reconnu = 0;

		}else if(strncmp(clt->commandeName_Recep, "ALIVE", strlen("ALIVE"))==0){
			commande_alive_RECEP(file, clt);

    /* Demande de permission de modifier une line */
		//EDITF-LIGNE
		}else if(strncmp(clt->commandeName_Recep, "EDITF", strlen("EDITF"))==0){
			commande_edit_RECEP(clt);
			nb_commande_non_reconnu = 0;

 /* Insertion de ligne*/
    	//INSRT-LIGNE
		}else if(strncmp(clt->commandeName_Recep, "INSRT", strlen("INSRT"))==0){
			commande_insert_RECEP(clt);
			nb_commande_non_reconnu = 0;

		/* Broadcast de la sauvegarde Final*/
    	//SAVEF-LIGNE-SIZE-BUFFER
		}else if(strncmp(clt->commandeName_Recep, "SAVET", strlen("SAVET"))==0){
            commande_save_RECEP(file, clt, 1);
            nb_commande_non_reconnu = 0;

    /* Broadcast de la sauvegarde Final*/
    	//SAVEF-LIGNE-SIZE-BUFFER
		}else if(strncmp(clt->commandeName_Recep, "SAVEF", strlen("SAVEF"))==0){
            commande_save_RECEP(file, clt, 0);
            nb_commande_non_reconnu = 0;

	/* Annulation d'édition */
		//ANNUL-LIGNE
		}else if(strncmp(clt->commandeName_Recep, "ANNUL", strlen("ANNUL"))==0){
			commande_annul_RECEP(file, clt);
			nb_commande_non_reconnu = 0;

    /* Suppression d'une line */
		//DELET-LIGNE
        }else if(strncmp(clt->commandeName_Recep, "DELET", strlen("DELET"))==0){
        	commande_delete_RECEP(file, clt);
        	nb_commande_non_reconnu = 0;

  	/* Fermeture du fichier */
    	//CLOSE
		}else if(strncmp(clt->commandeName_Recep, "CLOSE", strlen("CLOSE"))==0){
			nb_commande_non_reconnu = 0;
	  	   	break;

		}else{
			int car_chelou;
			car_chelou = atoi(clt->commandeName_Recep);
			printf("Thread recepteur : ERREUR Commande '%d'non connu \n", car_chelou);
			nb_commande_non_reconnu++;
			if(nb_commande_non_reconnu == 10){
				break;
			}
		}
	}

/* Si le client a crashé */
	if(clt->en_cours_dedition != 0){
		signal_commande_emission(clt, "ANNUL", clt->en_cours_dedition, 0, NULL);
		pthread_mutex_lock(&file->mutexTabFdSock);
		clt->en_cours_dedition = 0;
		pthread_mutex_unlock(&file->mutexTabFdSock);
	}

	signal_commande_emission(clt, "CLOSE", 0, 0, NULL);

    free(clt->commandeName_Recep);
    clt->commandeName_Recep = NULL;
    printf("***********************Thread Reception : TERMINAISON\n");
	pthread_exit (NULL);
}


//OK
/* Signalé au THREAD EMISSION */
/* traitement de la reception d'une commande d'ouveture d'un fichier */
/***************************************************************************/
struct fichier *commande_open_RECEP(struct client *clt){

    struct fichier *file;
    char *fileName, *fileName_reel;
    int taille;

    if(read(clt->fdSockReception, &taille, sizeof(int))==-1){
        perror("Thread recepteur : Probleme read taille OPENF");
        goto error_out_1;
    }

    fileName = initialiser_buffer(taille);

    if(read(clt->fdSockReception, fileName, taille)==-1){
        perror("Thread recepteur : Probleme read fileName OPENF");
        goto error_out_2;
    }

    fileName_reel = initialiser_buffer(taille + 1);
    snprintf(fileName_reel, (taille + 1), "%s", fileName);

    printf("Thread recepteur : Nom du fichier reçu = %s\n", fileName);

    file = trouver_Fichier(fileName_reel);

    clt->fileName = initialiser_buffer(taille + 1);
    snprintf(clt->fileName, (taille + 1), "%s", file->name);

    pthread_mutex_lock(&liste_file.mutex);
    supprimer_client_liste_temporaire(clt);
    pthread_mutex_unlock(&liste_file.mutex);

    ajouter_client_dans_fichier(file, clt);

    signal_commande_emission(clt, "OPENF", 0, taille, NULL);
    free(fileName);
    free(fileName_reel);

    return file;


error_out_2 :
    free(fileName);

error_out_1 :
    exit(-1);
}


//OK
/* Signalé au THREAD EMISSION */
/* traitement de la reception d'une commande d'une
 	demande d'édition d'une ligne du fichier 	*/
/************************************************/
void commande_edit_RECEP(struct client *clt){

	int ligne;

	if(read(clt->fdSockReception, &ligne, sizeof(int))==-1){
		perror("Thread recepteur : Probleme read ligne EDITF"); exit(-1);
	}

    signal_commande_emission(clt, "EDITF", ligne, 0, NULL);
}


//OK
/* Signalé au THREAD EMISSION */
/* Traitement reception d'une commande d'annulation d'une ligne en cours d'édition */
/***********************************************************************************/
void commande_annul_RECEP(struct fichier *file, struct client *clt){

	int ligne;

	if(read(clt->fdSockReception, &ligne, sizeof(int))==-1){
		perror("Thread recepteur : Probleme read ligne EDITF"); exit(-1);
	}
	printf("Thread recepteur ANNUL Line : %d\n", ligne);

    signal_commande_emission(clt, "ANNUL", ligne, 0, NULL);

	pthread_mutex_lock(&file->mutexTabFdSock);
	clt->en_cours_dedition = 0;
	pthread_mutex_unlock(&file->mutexTabFdSock);
}


//OK
/* Reception d'une requete d'insertion de ligne */
/************************************************/
void commande_insert_RECEP(struct client *clt){

	int ligne;

	if(read(clt->fdSockReception, &ligne, sizeof(int))==-1){
		perror("Thread recepteur : Probleme read ligne DELETE"); exit(-1);
	}

    signaler_thread_broadcast(&liste_file, clt->fdSockReception,
    						"INSRT", ligne, 0, NULL, clt->fileName);
}


//OK
/* Signalé au THREAD BROADCAST */
/* traitement de la reception d'une commande delete d'une ligne du fichier */
/***************************************************************************/
void commande_delete_RECEP(struct fichier *file, struct client *clt){

	int ligne;

	if(read(clt->fdSockReception, &ligne, sizeof(int))==-1){
		perror("Thread recepteur : Probleme read ligne DELETE"); exit(-1);
	}

    signaler_thread_broadcast(&liste_file, clt->fdSockReception, "DELET",
    										ligne, 0, NULL, clt->fileName);

    pthread_mutex_lock(&file->mutexTabFdSock);
	clt->en_cours_dedition = 0;
	pthread_mutex_unlock(&file->mutexTabFdSock);
}


//OK
/* Reception d'un message disant que le client est en vie */
/**********************************************************/
void commande_alive_RECEP(struct fichier *file, struct client *clt){

	if(file == NULL){
		pthread_mutex_lock(&liste_file.mutex);
		clt->client_en_vie++;
		pthread_mutex_unlock(&liste_file.mutex);

	}else{
		pthread_mutex_lock(&file->mutexTabFdSock);
		clt->client_en_vie++;
		pthread_mutex_unlock(&file->mutexTabFdSock);
	}
}



//OK
/* Signalé au THREAD BROADCAST */
/* traitement de la reception d'une commande save d'une ligne du fichier */
/*************************************************************************/
void commande_save_RECEP(struct fichier *file, struct client *clt, int tmp){

	int ligne, taille;
	char *buffer, *buf;

	ligne = reception_entier(clt->fdSockReception);
	taille = reception_entier(clt->fdSockReception);

	printf("Thread reception --> ligne : %d taille : %d\n", ligne, taille);

	if(taille != 0){
		taille++;	/* pour le '\0' */
	}

	buffer = initialiser_buffer(taille);
	if(read(clt->fdSockReception, buffer, taille)==-1){
		perror("Thread recepteur : Probleme read buffer");
		goto error_out_2;
	}

	buf = initialiser_buffer(taille);
	snprintf(buf, taille, "%s", buffer);

	if(taille > 0){
		taille--;
	}
	if(tmp == 1){
		printf("Thread Reception : SAVET ligne num : %d taille : %d buffer : '%s'\n",
									ligne, taille, buffer);

		signaler_thread_broadcast(&liste_file, clt->fdSockReception,
									"SAVET", ligne, taille, buffer, clt->fileName);
	}else{

		printf("Thread Reception : SAVEF ligne num : %d taille : %d buffer : '%s'\n",
									ligne, taille, buffer);

		signaler_thread_broadcast(&liste_file, clt->fdSockReception,
									"SAVEF", ligne, taille, buffer, clt->fileName);

		pthread_mutex_lock(&file->mutexTabFdSock);
		clt->en_cours_dedition = 0;
		pthread_mutex_unlock(&file->mutexTabFdSock);
	}

	free(buffer);

	return;

error_out_2 :
	free(buffer);
	exit(-1);
}


//OK
/* Chaque  envoie d'entier par le serveur se termine  par un underscore (_) */
/* Cette Fonction permet d'extraire l'entier du message reçu par le Serveur */
/****************************************************************************/
int reception_entier(int fdSockClientReception){

	char *carac, *tmp;
	int entier;

	carac = initialiser_buffer(2);
	tmp = initialiser_buffer(10);

	while(read(fdSockClientReception, carac, sizeof(char))){
		if((strcmp(carac, "_") == 0) || (strcmp(carac, "\0") == 0))
			break;

		strncat(tmp, carac, 1);
		memset(carac, 0, 2);
	}

	entier = atoi(tmp);
	free(carac);
	free(tmp);
	return entier;
}



//OK
/* Signaler une reception de commande au thread emission */
/*********************************************************/
void signal_commande_emission(struct client *clt, char *type, int ligne,
												int taille, char *buffer){
/* Je vérifie que le thread emission a traité lancienne commande */
	pthread_mutex_lock(&clt->mutexCommande);

	if(clt->commandeInfo.rempli == 1){
		pthread_cond_wait(&clt->condCommande, &clt->mutexCommande);
	}

/* REMPLIR LES CHAMPS DE LA STRUCTURE */
	strcpy(clt->commandeInfo.commandeName, type);
	clt->commandeInfo.ligne.num = ligne;
	clt->commandeInfo.ligne.taille = taille;

	if(buffer != NULL){

		clt->commandeInfo.ligne.buffer = initialiser_buffer(taille);

		memset(clt->commandeInfo.ligne.buffer, 0, taille);
		strncpy(clt->commandeInfo.ligne.buffer, buffer, taille);
	}else{
		clt->commandeInfo.ligne.buffer = NULL;
	}

/* Je reveille le thread emission */
	clt->commandeInfo.rempli=1;
	pthread_cond_signal(&clt->condCommande);
	pthread_mutex_unlock(&clt->mutexCommande);

	printf("Thread Reception : Signalé au thread emission \n");
}
