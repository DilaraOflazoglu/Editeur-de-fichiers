#ifndef FONCTIONS_SAUVEGARDE_CLIENT_H
#define FONCTIONS_SAUVEGARDE_CLIENT_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "structures.h"
#include "editeur_gtk.h"


/* Création et initialisation d'une nouvelle ligne dans la liste fichier->listeligne */
/*************************************************************************************/
struct ligneC *nouvelle_ligne(int num, int taille, char *buffer);


/* Création dans la liste fichier->ListeligneModif
Création sans initialisation du buffer __Appel EDITF__  ça parmettera de créer un 
champs liste et n'allouer le buffer qu'une fois __l'Appel SAVEF__ est reçu */
/***************************************************************************/
void nouvelle_ligne_vide(struct liste_ligneC *liste, int num);


/* Traitement d'ajout d'une ligne non existante dans le fichier file */
/*********************************************************************/
void remplir_ligne(struct ligneC *cible, int num, int taille,char *buffer);


/* Traitement d'une sauvegarde d'une ligne existante dans le fichier file */
/**************************************************************************/
void sauvegarder_ligne_existante(struct liste_ligneC *listemodif,
		 		struct liste_ligneC *liste, int num, int taille, char *buffer);
								

/* Ajout d'une ligne dans le tableau de lignes du fichier file */
/***************************************************************/
void ajouter_ligne(struct liste_ligneC *listemodif, 
					struct liste_ligneC *liste, struct ligneC *new);


/* Insertion en Queue de liste */
/*******************************/
void ajout_en_queue(struct liste_ligneC *liste, struct ligneC *new);

void sauvegarde_temporaire(struct liste_ligneC *liste, int num,
												int taille, char *buffer);

void trouver_et_remplir_ligne(int num, int taille, char *buffer);


/* Insertion d'une même ligne */
/******************************/
void ajout_avec_decalage(struct liste_ligneC *listemodif,
					struct liste_ligneC *liste, struct ligneC *new);


/* Free de la struct char et de la struct ligne */
/************************************************/
void detruire(struct ligneC *ligne);


/* Suppression d'une ligne du tableau file->tabLigne */
/*****************************************************/
void supprimer_ligne(struct ligneC *ligne);


/* Suppression d'une ligne des 2 listes chainées de ligne */
/**********************************************************/
void suppriession_dans_deux_listes(struct liste_ligneC *listemodif, 
									struct liste_ligneC *liste, int num_ligne);


/* Suppression de toutes les lignes de la liste et destruction de la structure */
/*******************************************************************************/
void destruction_liste_ligne(struct liste_ligneC *liste);


/* Verification si liste contient la ligne de numero num */
/*********************************************************/
int contains_ligne(struct liste_ligneC *liste, int num);


/* Décallage de tous les numeros de ligne de 1 à partir de ligne */
/*****************************************************************/
void decalage_num(struct liste_ligneC *listemodif, 
			struct liste_ligneC *liste, struct ligneC *ligne, int decalage);


/* Fonction est invoquée dans decalage_num, elle permet de verfier 
   si les numeros de ligne qu'on decale dans ListeLigne sont presentes
   dans ListeLigneModif pour les decaler aussi ! Elle permet aussi de 
   mettre à jour la ligne que l'utilisateur est entrain d'editer */
/*****************************************************************/
void verif_liste_ligne_modif(struct liste_ligneC *listemodif, 
												 int num, int decalage);


/* Affichage de tous les champs de la ligne passée en paramètre */
/****************************************************************/
void display_ligne(struct ligneC *ligne);


/* Affichage de toutes les lignes de la liste de ligne */
/*******************************************************/
void display_liste(struct liste_ligneC *liste);


/* Retourne le nombre de caratere d'une ligne avant édition ligne */
/******************************************************************/
int nb_caractere_ligne(struct liste_ligneC *liste, int ligne);


/* Cette fonction Retourne le nombre de  caratere d'une ligne */
/* strlen sur les carateres speciaux, elle renvoie 2 et nom 1 */
/**************************************************************/
int nb_caractere_reel_ligne(char *buffer);


#endif