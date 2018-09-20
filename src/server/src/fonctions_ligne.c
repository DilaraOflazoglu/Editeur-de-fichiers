#include "fonctions_ligne.h"


//OK
/* Création et initialisation d'une nouvelle ligne dans la liste fichier->listeligne */
/*************************************************************************************/
struct ligneS *nouvelle_ligne(int num, int taille, char *buffer){	//OK

	struct ligneS* ligne;

	if((ligne = (struct ligneS *)malloc(sizeof(struct ligneS))) == NULL){
		perror("Création LigneS : PB malloc LigneS !"); exit(-1);
	}

	if(buffer != NULL){
		ligne->buffer = initialiser_buffer(taille + 1);
		snprintf(ligne->buffer, (taille + 1), "%s", buffer);
	}

	ligne->num = num;
	ligne->taille = taille;
	ligne->next = ligne;
	ligne->prev = ligne;

	return ligne;
}


//OK
/* Attention !!! Verrou pris dans la fonction commande_edit_Em */
/* Création dans la liste fichier->ListeligneModif
Création sans initialisation du buffer __Appel EDITF__  ça parmettera de crérer
un champs liste et n'allouer le buffer qu'une fois __l'Appel SAVEF__ est reçu */
/****************************************************************/
void nouvelle_ligne_vide(struct liste_ligneS *liste, int num){

	struct ligneS* ligne;

	if((ligne = (struct ligneS*)malloc(sizeof(struct ligneS))) == NULL){
		perror("fonctions_ligne : Erreur malloc ligne");  exit(-1);
	}

	ligne->num = num;
	ligne->taille = 0;
	ligne->buffer = NULL;

/* ajout à la fin de listeLigneModif de file*/
	ligne->next = &liste->tabLigne;
	ligne->prev = liste->tabLigne.prev;
	liste->tabLigne.prev->next = ligne;
	liste->tabLigne.prev = ligne;
}


//OK
/* Ajout d'une ligne dans la liste de ligne precisé */
/****************************************************/
void ajouter_ligne(struct fichier *file, int fdClient, struct ligneS *new){

	int last_num;

	pthread_mutex_lock(&file->listeLigne.mutexListeLigne);
	last_num = file->listeLigne.tabLigne.prev->num;

/* Insertion en queue */
	if(new->num == (last_num + 1)){
		ajout_en_queue(&file->listeLigne, new);
	}

/* Insertion d'une ligne qui existe */
	else{
		pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);
		ajout_avec_decalage(file, fdClient, new);
		pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);
	}
	pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
}


//OK
/* Cas : Insertion en Queue de liste */
/* On ajoute 4  dans liste qui contient ligne 0 1 2 3, insertion apres 3 */
/*************************************************************************/
void ajout_en_queue(struct liste_ligneS *liste, struct ligneS *new){

	new->next = &liste->tabLigne;
	new->prev = liste->tabLigne.prev;
	liste->tabLigne.prev->next = new;
	liste->tabLigne.prev = new;
}


//OK
/* Cas : Insertion d'une même ligne */
/* On ajoute 4  dans liste qui contient ligne 0 1 2 4 */
/* Insertion d'une nouvelle ligne au num 4 et on decalle les autres */
/********************************************************************/
void ajout_avec_decalage(struct fichier *file, int fdClient, struct ligneS *new){

	struct ligneS *cible;

	cible = file->listeLigne.tabLigne.next;

	while(cible != &file->listeLigne.tabLigne){

		if(cible->num == new->num){
			decalage_num(file, fdClient, cible, 1);
			new->next = cible;
			new->prev = cible->prev;
			cible->prev->next = new;
			cible->prev = new;

			break;
		}
		cible = cible->next;
	}
}


//OK
/* Decalage des numeros des lignes en fonction de l'ajout ou de la suppression */
/*******************************************************************************/
void decalage_num(struct fichier *file, int fdClient,
									struct ligneS *ligne, int decalage){

	struct ligneS *cible;

/* Ajout de ligne */
	if(decalage == 1){
		ligne->num++;
		cible = file->listeLigne.tabLigne.prev;
		while(cible != ligne){
			verif_liste_ligne_modif(&file->listeLigneModif, cible->num, decalage);
			verif_en_cours_dedition_client(file, fdClient, cible->num, decalage);
			cible->num += decalage;
			cible = cible->prev;
		}
	}

/* Suppression de ligne */
	else{
		cible = ligne->next;
		while(cible != &file->listeLigne.tabLigne){
			printf("DEBUG --> cible->num : %d\n", cible->num);
			verif_liste_ligne_modif(&file->listeLigneModif, cible->num, decalage);
			verif_en_cours_dedition_client(file, fdClient, cible->num, decalage);
			cible->num += decalage;
			cible = cible->next;
		}
	}
}


//OK
/* Fonction est invoquée dans decalage_num, elle permet de verfier
   si les numeros de ligne qu'on decale dans ListeLigne sont presentes
   dans ListeLigneModif pour les decaler aussi ! Elle permet aussi de
   mettre à jour la ligne que l'utilisateur est entrain d'editer */
/* ATTENTION !!!! Mutex pris avant l'invocation de ajouter_ligne */
/*****************************************************************/
void verif_liste_ligne_modif(struct liste_ligneS *listeModif,
											 int num, int decalage){
	struct ligneS *cible;

	cible = listeModif->tabLigne.next;

	while(cible != &listeModif->tabLigne){
		if(cible->num == num){
			cible->num += decalage;
			return;
		}

		cible = cible->next;
	}
}


//OK
/* Traitement d'une sauvegarde d'une ligne existante dans le fichier file */
/**************************************************************************/
void sauvegarder_ligne_existante(struct liste_ligneS *listemodif,
		 		struct liste_ligneS *liste, int num, int taille, char *buffer){

	struct ligneS *curseur;

	pthread_mutex_lock(&liste->mutexListeLigne);
	curseur = liste->tabLigne.next;

	while(curseur != &liste->tabLigne){
		if(curseur->num == num){
			if(curseur->taille != 0){
				free(curseur->buffer);
			}
			remplir_ligne(curseur, num, taille, buffer);
			break;
		}

		curseur = curseur->next;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);


	/* Supprimer de la liste ligne modif */
	pthread_mutex_lock(&listemodif->mutexListeLigne);
	curseur = listemodif->tabLigne.next;

	while(curseur != &listemodif->tabLigne){
        if(curseur->num == num){
		/* Suppression, remplissage des données et MAJ du chainage */
        	supprimer_ligne(curseur);
        	detruire(curseur);
        	break;
        }
            curseur = curseur->next;
   	}
	pthread_mutex_unlock(&listemodif->mutexListeLigne);
}


//OK
/* Enregistrement en temps réelle dans la liste de ligne */
/*********************************************************/
void sauvegarde_temporaire(struct liste_ligneS *liste, int num,
												int taille, char *buffer){

	struct ligneS *curseur;

	pthread_mutex_lock(&liste->mutexListeLigne);
	curseur = liste->tabLigne.next;

	while(curseur != &liste->tabLigne){
		if(curseur->num == num){
			if(curseur->taille != 0){
				free(curseur->buffer);
			}
			remplir_ligne(curseur, num, taille, buffer);
			break;
		}

		curseur = curseur->next;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);
}


/* Remplir le cache pour garder une copie de la ligne avant une modif par l'user */
/*********************************************************************************/
void remplir_cache(struct liste_ligneS *liste, struct client *clt, int num){

	struct ligneS *curseur;

	pthread_mutex_lock(&liste->mutexListeLigne);
	curseur = liste->tabLigne.next;

	while(curseur != &liste->tabLigne){
		if(curseur->num == num){
			if(curseur->taille != 0){
				clt->cache = initialiser_buffer(curseur->taille + 1);
				snprintf(clt->cache, (curseur->taille + 1), "%s", curseur->buffer);
			}
			break;
		}
		curseur = curseur->next;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);
}


//OK
/* Traitement d'ajout d'une ligne non existante dans le fichier file */
/*********************************************************************/
void remplir_ligne(struct ligneS *cible,
								int num, int taille, char *buffer){

	cible->num = num;

	if(buffer == NULL){
		cible->taille = 0;
		cible->buffer = NULL;
	}

	else{
		cible->taille=strlen(buffer);
		cible->buffer = initialiser_buffer(taille + 1);
		snprintf(cible->buffer, taille + 1, "%s", buffer);
	}
}


//OK
/* Free de la struct char et de la struct ligne */
/************************************************/
void detruire(struct ligneS *ligne){
	if(ligne->buffer != NULL)
		free(ligne->buffer);

	free(ligne);
}


//OK
/* Suppression d'une ligne du tableau file->tabLigne */
/* Attention !!!!  Cette fonction est a utliser entre mutex dans une boucle ! */
/******************************************************************************/
void supprimer_ligne(struct ligneS *ligne){
	ligne->prev->next=ligne->next;
	ligne->next->prev=ligne->prev;
}


//OK
/* Preparation du message d'une suppression de ligne a tous les utilisateurs */
/*****************************************************************************/
void suppriession_dans_deux_listes(struct fichier *file, int fdClient, int num_ligne){
	struct ligneS *cible;

/* Supprimer de la liste ligne modif */
	pthread_mutex_lock(&file->listeLigneModif.mutexListeLigne);
	cible = file->listeLigneModif.tabLigne.next;

	while(cible != &file->listeLigneModif.tabLigne){
        if(cible->num == num_ligne){
	/* Suppression, remplissage des données et MAJ du chainage */
        	supprimer_ligne(cible);
        	detruire(cible);
        	break;
        }
            cible = cible->next;
   	}
	pthread_mutex_unlock(&file->listeLigneModif.mutexListeLigne);

/* Suppression de la liste ligne */
	pthread_mutex_lock(&file->listeLigne.mutexListeLigne);
	cible = file->listeLigne.tabLigne.prev;

	while(cible != &file->listeLigne.tabLigne){
		if(cible->num == num_ligne){
			decalage_num(file, fdClient, cible, -1);
			supprimer_ligne(cible);
			detruire(cible);
			break;
		}
		cible = cible->prev;
	}
	pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
}


//OK
/* Retourne 0 si la struct liste_ligneS ne contient pas la struct ligne, 1 sinon */
/*********************************************************************************/
int contains_ligne(struct liste_ligneS *liste, int num){

	struct ligneS *cible;

	pthread_mutex_lock(&liste->mutexListeLigne);
	cible = liste->tabLigne.next;

	while(cible != &liste->tabLigne){

		if(cible->num == num){
			pthread_mutex_unlock(&liste->mutexListeLigne);
			return 1;
		}

		cible = cible->next;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);
	return 0;
}


//OK
/* Affichage d'une ligne precise */
/*********************************/
void display_ligne(struct ligneS *ligne){

	if(ligne->taille != 0){
		printf("ligne numero : %d taille : %d contenu : %s\n",
									ligne->num, ligne->taille, ligne->buffer);
	}else{
		printf("ligne numero : %d taille : %d contenu : '(nulle)'\n",
									ligne->num, ligne->taille);
	}
}


//OK
/* Affichage de la totalite de la liste de ligne */
/*************************************************/
void display_liste(struct liste_ligneS *liste){

	struct ligneS *cible;

	pthread_mutex_lock(&liste->mutexListeLigne);
	cible = liste->tabLigne.next;

	while(cible != &liste->tabLigne){

		display_ligne(cible);

		cible = cible->next;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);
}



/* Suppression de toutes les lignes de la liste et destruction de la structure */
/*******************************************************************************/
void destruction_liste_ligne(struct liste_ligneS *liste){

	struct ligneS *curseur, *cible;

	pthread_mutex_lock(&liste->mutexListeLigne);

	curseur = &liste->tabLigne;
	cible = curseur;
	curseur = curseur->next;	/* Sinon on risque de perdre le pointeur sur le suivant */

	while(curseur != &liste->tabLigne){
			cible = curseur;
			curseur = curseur->next;

			supprimer_ligne(cible);
			detruire(cible);
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);

	pthread_mutex_destroy(&liste->mutexListeLigne);
}
