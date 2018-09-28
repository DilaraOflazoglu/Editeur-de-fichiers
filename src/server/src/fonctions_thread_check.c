#include "fonctions_thread_check.h"


//OK
/* Parcours de chaque fichier puis attente de "15 secondes" */
/************************************************************/
void *check(){
	int i;
//Attention cas d'arrêt a gerer
	while(1){

		sleep(60);
		check_liste_temporaire();

		for(i=0; i<NBFILE; i++){
			check_one_file(&liste_file.liste[i]);
		}
	}
}


//OK
/* Parcours de liste chainée pour supprimer les clients déconnectés */
/********************************************************************/
void check_one_file(struct fichier *file){

	printf("Thread check --> check file : %s\n", file->name);
	struct client *curseur, *cible;

	pthread_mutex_lock(&file->mutexTabFdSock);

	curseur = &file->fdSocks;
	cible = curseur;
/* Sinon on risque de perdre le pointeur sur le suivant */
	curseur = curseur->next;

	while(curseur != &file->fdSocks){
		cible = curseur;
		curseur = curseur->next;

		printf("Thread check client->fdEmission : %d deconnection = %d\n",
							cible->fdSockEmission, cible->client_deconnecte);

/* Client Deconnecté */
		if(cible->client_deconnecte == 1){
			if(cible->client_en_panne == 0){
				suppression_client_en_panne(file, cible);
			}
		}

/* Client Connecté */
	/* Cas ou le thread Check se rend compte que le client ne repond plus */
		else{
		/* Cas critique --> client semble en panne */
			if(cible->client_en_vie < 2){
				cible->client_en_vie = 0;
				cible->client_en_panne++;

				printf("Thread check --> cas CRITIQUE n° : %d\n",
														cible->client_en_panne);

				if(cible->client_en_panne == 3){
					suppression_client_en_panne(file, cible);
				}
			}

		/* Tout va bien client en vie */
			else{
				cible->client_en_panne = 0;
				cible->client_en_vie = 0;
			}
		}
	}
	pthread_mutex_unlock(&file->mutexTabFdSock);
}


//OK
/* Parcours de liste chainée temporaire pour supprimer les clients déconnectés */
/*******************************************************************************/
void check_liste_temporaire(){

	printf("Thread check --> check liste temporaire \n");
	struct client *curseur, *cible;

	pthread_mutex_lock(&liste_file.mutex);

	curseur = &liste_file.listeClientTmp;
	cible = curseur;
/* Sinon on risque de perdre le pointeur sur le suivant */
	curseur = curseur->next;

	while(curseur != &liste_file.listeClientTmp){
			cible = curseur;
			curseur = curseur->next;

		printf("Thread check client->fdEmission : %d deconnection = %d\n",
							cible->fdSockEmission, cible->client_deconnecte);

/* Client Deconnecté */
		if(cible->client_deconnecte == 1){
			if(cible->client_en_panne == 0){
				supprimer_client_liste_temporaire(cible);
				detruire_client(cible);
			}
		}

/* Client Connecté */
	/* Cas ou le thread Check se rend compte que le client ne repond plus */
		else{
		/* Cas critique --> client semble en panne */
			if(cible->client_en_vie < 2){

				cible->client_en_vie = 0;
				cible->client_en_panne++;
				printf("Thread check --> cas CRITIQUE n° : %d\n",
													cible->client_en_panne);

				if(cible->client_en_panne == 3){
					printf("Thread check --> CLIENT en PANNE\n");
					pthread_cancel(cible->thread_reception);
					pthread_cancel(cible->thread_emission);
					supprimer_client_liste_temporaire(cible);
					detruire_client(cible);
				}
			}

		/* Tout va bien client en vie */
			else{
				cible->client_en_panne = 0;
				cible->client_en_vie = 0;
			}
		}
	}
	pthread_mutex_unlock(&liste_file.mutex);
}


//OK
/* Suppression d'un client de la liste des clients qui est tombé en panne */
/* Verification que le client n'est pas en cours d'édition sinon on envoie
	un message d'annulation a tous les autres clients */
/**************************************************************************/
void suppression_client_en_panne(struct fichier *file, struct client *clt){

	int taille;
	char *envoieMess;
	struct client *cible;
	struct ligneS *ligne;

	pthread_cancel(clt->thread_reception);
	pthread_cancel(clt->thread_emission);

	if(clt->en_cours_dedition != 0){

		taille = strlen("ANNUL") + sizeof(int);

		envoieMess = initialiser_buffer(taille);

		snprintf(envoieMess, taille, "ANNUL%d", clt->en_cours_dedition);

	/* Envoie de l'annulation a tous les clients */
		printf("Thread Broadcast Without --> envoie du message : '%s'\n", envoieMess);

		cible = file->fdSocks.next;

		while(cible != &file->fdSocks){
			if((cible->fdSockEmission != clt->fdSockEmission)
			&& (cible->fdSockReception != clt->fdSockReception)){

				if(cible->client_deconnecte == 0){
					if(write(cible->fdSockEmission, envoieMess,
											strlen(envoieMess)) == -1){
						cible->client_deconnecte = 1;
						cible->client_en_panne = 0;
					}
				}
			}
			cible = cible->next;
		}
		free(envoieMess);

	/* Suppression de la ligne de la liste ligne Modif */
		pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);
		ligne = file->listeLigneModif.tabLigne.next;

		while(ligne !=  &file->listeLigneModif.tabLigne){
			if(ligne->num == clt->en_cours_dedition){
				supprimer_ligne(ligne);
				free(ligne);
				break;
			}
			ligne = ligne->next;
		}
		pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);
	}

	supprimer_client(clt);
	detruire_client(clt);
}