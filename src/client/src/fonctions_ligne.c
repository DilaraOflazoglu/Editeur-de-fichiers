#include "fonctions_ligne.h"


//OK
/* Création et initialisation d'une nouvelle ligne dans la liste fichier->listeligne */
/*************************************************************************************/
struct ligneC *nouvelle_ligne(int num, int taille, char *buffer){	//OK

	struct ligneC* ligne;

	if((ligne=(struct ligneC *)malloc(sizeof(struct ligneC)))==NULL){
		perror("fonctions_ligne : PB malloc LigneC !"); exit(-1);
	}

	if(buffer != NULL){
		ligne->buffer = initialiser_buffer(taille);
		memset(ligne->buffer, 0, taille);
		snprintf(ligne->buffer, taille+1, "%s", buffer);
	}

	ligne->num = num;
	ligne->taille = taille;
	ligne->next = ligne;
	ligne->prev = ligne;

	return ligne;
}


//OK
/* Création dans la liste fichier->ListeligneModif
Création sans initialisation du buffer __Appel EDITF__  ça parmettera de crérer
un champs liste et n'allouer le buffer qu'une fois __l'Appel SAVEF__ est reçu */
/****************************************************************/
void nouvelle_ligne_vide(struct liste_ligneC *liste, int num){

	struct ligneC* ligne;

	if((ligne = (struct ligneC*)malloc(sizeof(struct ligneC))) == NULL){
		perror("fonctions_ligne : Erreur malloc ligne");  exit(-1);
	}

	ligne->num = num;
	ligne->taille = 0;
	ligne->buffer = NULL;

	pthread_mutex_lock(&liste->mutexListeLigne);
/* ajout à la fin de listeLigneModif de file*/
	ligne->next = &liste->tabLigne;
	ligne->prev = liste->tabLigne.prev;
	liste->tabLigne.prev->next = ligne;
	liste->tabLigne.prev = ligne;

	pthread_mutex_unlock(&liste->mutexListeLigne);
}


/* Ajout d'une ligne dans la liste de ligne precisé */
/****************************************************/
void ajouter_ligne(struct liste_ligneC *listemodif,
					struct liste_ligneC *liste, struct ligneC *new){

	int last_num;

	last_num = liste->tabLigne.prev->num;

/* Insertion en queue */
	if(new->num == (last_num + 1)){
		ajout_en_queue(liste, new);
	}

/* Insertion d'une ligne qui existe */
	else{
		ajout_avec_decalage(listemodif, liste, new);
	}
}


//OK
/* Cas : Insertion en Queue de liste */
/* On ajoute 4  dans liste qui contient ligne 0 1 2 3, insertion apres 3 */
/*************************************************************************/
void ajout_en_queue(struct liste_ligneC *liste, struct ligneC *new){
	new->next = &liste->tabLigne;
	new->prev = liste->tabLigne.prev;
	liste->tabLigne.prev->next = new;
	liste->tabLigne.prev = new;
}


//OK
/* Cas : Insertion d'une même ligne */
/* On ajoute 4  dans liste qui contient ligne 0 1 2 4 */
/* Insertion d'une nouvelle ligne au num 5 et on decalle les autres */
/********************************************************************/
void ajout_avec_decalage(struct liste_ligneC *listemodif,
					struct liste_ligneC *liste, struct ligneC *new){

	struct ligneC *cible;

	cible = liste->tabLigne.next;

	while(cible != &liste->tabLigne){

		if(cible->num == new->num){
			decalage_num(listemodif, liste, cible, 1);

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
void decalage_num(struct liste_ligneC *listemodif,
				struct liste_ligneC *liste, struct ligneC *ligne, int decalage){

	struct ligneC *cible;

/* Ajout de ligne */
	if(decalage == 1){
		ligne->num++;
		cible = liste->tabLigne.prev;
		while(cible != ligne){
			verif_liste_ligne_modif(listemodif, cible->num, decalage);
			cible->num += decalage;
			cible = cible->prev;
		}
	}

/* Suppression de ligne */
	else{
		cible = ligne->next;
		while(cible !=  &liste->tabLigne){
			verif_liste_ligne_modif(listemodif, cible->num, decalage);
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
/*****************************************************************/
void verif_liste_ligne_modif(struct liste_ligneC *liste,
											 int num, int decalage){
	struct ligneC *cible;

	cible = liste->tabLigne.next;

	while(cible != &liste->tabLigne){

		if(cible->num == num){
			cible->num += decalage;
			pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
			if(fichierInfo->en_cours_dedition == num){
				fichierInfo->en_cours_dedition += decalage;
			}
			pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
			printf("NOUVEAU NUMERO DE LIGNE : %d\n", cible->num);
			return;
		}
		cible = cible->next;
	}
}


//OK
/* Traitement d'une sauvegarde d'une ligne existante dans le fichier file */
/**************************************************************************/
void sauvegarder_ligne_existante(struct liste_ligneC *listemodif,
		 		struct liste_ligneC *liste, int num, int taille, char *buffer){

	struct ligneC *curseur;

	pthread_mutex_lock(&liste->mutexListeLigne);
	curseur = liste->tabLigne.next;

	while(curseur != &liste->tabLigne){
		if(curseur->num == num){
			if(curseur->buffer != NULL){
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
/* Lors d'un SAVET (temps reelle) cette methode rempli le buffer de ma ligne*/
/****************************************************************************/
void trouver_et_remplir_ligne(int num, int end_offset, char *ecriture){

	struct ligneC *curseur;

	pthread_mutex_lock(&fichierInfo->listeLigne.mutexListeLigne);
	curseur = fichierInfo->listeLigne.tabLigne.next;

	while(curseur != &fichierInfo->listeLigne.tabLigne){
		if(curseur->num == fichierInfo->en_cours_dedition){
			if(curseur->taille != 0){
				free(curseur->buffer);
			}
			remplir_ligne(curseur, num, end_offset, ecriture);
			break;
		}
		curseur = curseur->next;
	}
	pthread_mutex_unlock(&fichierInfo->listeLigne.mutexListeLigne);
}


//OK
/* Lors d'un SAVET (temps reelle) cette methode rempli le buffer d'une ligne USER */
/**********************************************************************************/
void sauvegarde_temporaire(struct liste_ligneC *liste, int num,
												int taille, char *buffer){

	struct ligneC *curseur;

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


//OK
/* Traitement d'ajout d'une ligne non existante dans le fichier file */
/*********************************************************************/
void remplir_ligne(struct ligneC *cible, int num, int taille, char *buffer){

	cible->num = num;
	cible->taille = taille;

	cible->buffer = initialiser_buffer(taille + 1);

	snprintf(cible->buffer, taille + 1, "%s", buffer);

}


//OK
/* Free de la struct char et de la struct ligne */
/************************************************/
void detruire(struct ligneC *ligne){

	if(ligne->taille != 0){
		free(ligne->buffer);
	}
	free(ligne);
}


//OK
/* Suppression d'une ligne du tableau file->tabLigne */
/* Attention !!!!  Cette fonction est a utliser entre mutex dans une boucle ! */
/*****************************************************/
void supprimer_ligne(struct ligneC *ligne){

	ligne->prev->next=ligne->next;
	ligne->next->prev=ligne->prev;
}


//OK
/* Suppression d'une ligne des 2 listes chainées de ligne */
/**********************************************************/
void suppriession_dans_deux_listes(struct liste_ligneC *listemodif,
									struct liste_ligneC *liste, int num_ligne){

	struct ligneC *cible;

/* Supprimer de la liste ligne modif */
	pthread_mutex_lock(&listemodif->mutexListeLigne);
	cible = listemodif->tabLigne.next;

	while(cible != &listemodif->tabLigne){
        if(cible->num == num_ligne){
	/* Suppression, remplissage des données et MAJ du chainage */
        	supprimer_ligne(cible);
        	detruire(cible);
        	break;
        }
            cible = cible->next;
   	}
	pthread_mutex_unlock(&listemodif->mutexListeLigne);

/* Suppression de la liste ligne */
	pthread_mutex_lock(&liste->mutexListeLigne);
	cible = liste->tabLigne.prev;

	while(cible != &liste->tabLigne){
		if(cible->num == num_ligne){
			decalage_num(listemodif, liste, cible, -1);
			supprimer_ligne(cible);
			detruire(cible);
			break;
		}
		cible = cible->prev;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);

	printf("------------------------- MA LIGNE : %d\n\n", fichierInfo->en_cours_dedition);
	display_liste(listemodif);
	printf("------------------------- FIN LISTE LIGNE MODIF \n\n");
}


//OK
/* Retourne le nombre de caractere déjà present dans une ligne existante, 0 sinon */
/**********************************************************************************/
int nb_caractere_ligne(struct liste_ligneC *liste, int ligne){

	struct ligneC *cible;

	cible = liste->tabLigne.next;

	while(cible != &liste->tabLigne){
		if(cible->num == ligne){
			if(cible->taille == 0){
				return 0;
			}else{
				return nb_caractere_reel_ligne(cible->buffer);
			}
		}
		cible = cible->next;
	}
	return 0;
}


//OK
/* Cette fonction Retourne le nombre de  caratere d'une ligne */
/* strlen sur les carateres speciaux, elle renvoie 2 et non 1 */
/**************************************************************/
int nb_caractere_reel_ligne(char *buf){

	char *k;
	int first = 0;
	int taille;
	char *buffer;

	if(buf != NULL){
		taille = strlen(buf);
		buffer = initialiser_buffer(taille + 1);

		snprintf(buffer, (taille + 1), "%s", buf);

		k = strtok (buffer,"éèç£ê°ëàùµ§");

		while (k != NULL){
			first++;
		    k = strtok (NULL, "éèç£ê°ëàùµ§");
		}

		if(first > 1){
	   		taille-=(first - 1);
	   	}

		free(buffer);
	}else{
		taille = 0;
	}
	printf("----------->>>>> Fonctions_ligne --> taille ligne BASE = %d\n", taille);

	return taille;
}


//OK
/* Retourne 0 si la struct liste_ligneC ne contient pas la struct ligne, 1 sinon */
/*********************************************************************************/
int contains_ligne(struct liste_ligneC *liste, int num){

	struct ligneC *cible;

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
void display_ligne(struct ligneC *ligne){

	if(ligne->buffer != NULL){
		printf("ligne numero : %d taille : %d contenu : %s\n",
									ligne->num, ligne->taille, ligne->buffer);
	}else{
		printf("ligne numero : %d taille : %d contenu : (null)\n",
									ligne->num, ligne->taille);
	}
}


//OK
/* Affichage de la totalite de la liste de ligne */
/*************************************************/
void display_liste(struct liste_ligneC *liste){

	struct ligneC *cible;

	pthread_mutex_lock(&liste->mutexListeLigne);
	cible = liste->tabLigne.next;

	while(cible != &liste->tabLigne){

		display_ligne(cible);

		cible = cible->next;
	}
	pthread_mutex_unlock(&liste->mutexListeLigne);
}


//OK
/* Suppression de toutes les lignes de la liste et destruction de la structure */
/*******************************************************************************/
void destruction_liste_ligne(struct liste_ligneC *liste){

	struct ligneC *curseur, *cible;

	curseur = &liste->tabLigne;
	cible = curseur;
/* Sinon on risque de perdre le pointeur sur le suivant */
	curseur = curseur->next;

	while(curseur != &liste->tabLigne){
			cible = curseur;
			curseur = curseur->next;

			supprimer_ligne(cible);
			detruire(cible);
	}

	pthread_mutex_destroy(&liste->mutexListeLigne);
}