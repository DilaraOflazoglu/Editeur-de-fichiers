#include "fonctions_client.h"


/* Création et initialisation d'un nouveau pointeur de client */
/**************************************************************/
struct client *nouveau_client(int fdEmission, int fdReception){

	struct client *clt;

	if((clt = (struct client *)malloc(sizeof(struct client))) == NULL){
		perror("Fonctions Client : PB malloc struct client");
		goto error_malloc_client;
	}

	clt->fdSockEmission=fdEmission;
	clt->fdSockReception=fdReception;
	clt->en_cours_dedition = 0;
	clt->client_deconnecte = 0;
	clt->client_en_vie = 0;
	clt->client_en_panne = 0;

	clt->commandeName_Recep = NULL;
	clt->commandeName_Em = NULL;

	clt->buffer_Em = NULL;

	clt->fileName = NULL;
	clt->cache = NULL;

	clt->next = clt;
    clt->prev = clt;

	clt->commandeInfo.rempli=0;

	clt->commandeInfo.commandeName = initialiser_buffer(6);

    memset(clt->commandeInfo.commandeName, 0, 6);
    if(pthread_mutex_init(&clt->mutexCommande, NULL) != 0){
    	perror("Fonctions Client : PB mutex init");
    	goto error_mutex_init;
    }

	if(pthread_cond_init(&clt->condCommande, NULL) != 0){
		perror("Fonctions Client : PB cond init");
		goto error_cond_init;
	}

	return clt;

error_cond_init :
	pthread_mutex_destroy(&clt->mutexCommande);

error_mutex_init :
	free(clt->commandeInfo.commandeName);
	free(clt);

error_malloc_client :
	exit(-1);
}


/* Ajouter un client à la liste chainé */
/***************************************/
void ajouter_client_dans_fichier(struct fichier *file, struct client *new_clt){

    pthread_mutex_lock(&file->mutexTabFdSock);

	new_clt->next = &file->fdSocks;
	new_clt->prev = file->fdSocks.prev;
	file->fdSocks.prev->next = new_clt;
    file->fdSocks.prev = new_clt;

    pthread_mutex_unlock(&file->mutexTabFdSock);

    printf("Fonction client : Client ajouté dans le fichier avec succès \n");

}


/* Ajouter un client à la liste chainé temporaire */
/**************************************************/
void ajouter_client_liste_temporaire(struct client *new_clt){

    pthread_mutex_lock(&liste_file.mutex);
	new_clt->next = &liste_file.listeClientTmp;
	new_clt->prev = liste_file.listeClientTmp.prev;
	liste_file.listeClientTmp.prev->next = new_clt;
    liste_file.listeClientTmp.prev = new_clt;
    pthread_mutex_unlock(&liste_file.mutex);

    printf("Fonction client : Client ajouté dans la liste temporaire avec succès \n");
}


/* Le champs clientNonConcerne  represente le client qui a fait la modif  */
/* On décale le champs en_cours_dedition de tous les utlisateurs sauf lui */
/**************************************************************************/
void verif_en_cours_dedition_client(struct fichier *file, int clientNonConcerne,
												int num, int decalage){

	struct client *cible;

	pthread_mutex_lock(&file->mutexTabFdSock);
	cible = file->fdSocks.next;

	while(cible != &file->fdSocks){
		if((cible->en_cours_dedition == num) &&
			(cible->fdSockEmission != clientNonConcerne) &&
			(cible->fdSockReception != clientNonConcerne)){
			cible->en_cours_dedition += decalage;
			printf("cible->en_cours_dedition : %d\n", cible->en_cours_dedition);
		}
		cible = cible->next;
	}
	pthread_mutex_unlock(&file->mutexTabFdSock);
}


/* Ajouter un client à la liste chainé temporaire */
/**************************************************/
void supprimer_client_liste_temporaire(struct client *new_clt){

	new_clt->next->prev = new_clt->prev;
	new_clt->prev->next = new_clt->next;
}


/* Supprimer un client du tableau */
/**********************************/
void supprimer_client(struct client *clt){

/* Fonction utilisée par le thread check verrou
				pthread_mutex_lock(&file->mutexTabFdSock);
							deja pris lors de l'appel a cette fonction  */
  	clt->next->prev = clt->prev;
	clt->prev->next = clt->next;
}



/* Free la mémoire du pointeur d'un client */
/*******************************************/
void detruire_client(struct client *clt){

/* Buffer ligne supprimé dans la fonction "attente_signal_commande" du thread emission */

	if(clt->fileName != NULL)
		free(clt->fileName);
/* Attente des threads */
	if(pthread_join(clt->thread_emission, NULL) != 0){
		perror("Main : pb invocation pthread_join \n");	 exit(EXIT_FAILURE);
	}
	if(pthread_join(clt->thread_reception, NULL) != 0){
		perror("Main : pb invocation pthread_join \n");	 exit(EXIT_FAILURE);
	}

	if(clt->commandeInfo.commandeName != NULL)
		free(clt->commandeInfo.commandeName);


	if(clt->commandeName_Recep != NULL){
		free(clt->commandeName_Recep);
	}

	if(clt->commandeName_Em != NULL){
		free(clt->commandeName_Em);
	}

	if(clt->buffer_Em != NULL){
		free(clt->buffer_Em);
	}

	if(clt->cache != NULL){
		free(clt->cache);
	}

	fermeture_socket_tcp(clt->fdSockReception);
	fermeture_socket_tcp(clt->fdSockEmission);

	pthread_mutex_destroy(&clt->mutexCommande);
	pthread_cond_destroy(&clt->condCommande);

	free(clt);

	printf("Thread Check : Client supprimé et détruit avec succès \n");
}


/* Affichage de tous les clients */
/*********************************/
void display(struct fichier *file){

	struct client *cible;

	pthread_mutex_lock(&file->mutexTabFdSock);
	cible = file->fdSocks.next;

	while(cible != &file->fdSocks){
		printf("client fdEmission : %d fd reception : %d\n",
					cible->fdSockEmission, cible->fdSockReception);
		cible = cible->next;
	}
	pthread_mutex_unlock(&file->mutexTabFdSock);
}