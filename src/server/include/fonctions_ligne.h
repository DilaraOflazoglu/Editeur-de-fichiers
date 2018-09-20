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
#include "fonctions_client.h"
#include "fonctions_fichier.h"


/* Création et initialisation d'une nouvelle ligne dans la liste fichier->listeligne */
/***************************************************/
struct ligneS *nouvelle_ligne(int num, int taille, char *buffer);


/* Création dans la liste fichier->ListeligneModif
Création sans initialisation du buffer __Appel EDITF__  ça parmettera de créer un 
champs liste et n'allouer le buffer qu'une fois __l'Appel SAVEF__ est reçu */
/***************************************************************************/
void nouvelle_ligne_vide(struct liste_ligneS *liste, int num);


/* Ajout d'une ligne dans le tableau de lignes du fichier file */
/***************************************************************/
void ajouter_ligne(struct fichier *file, int fdClient, struct ligneS *new);


/* Insertion en Queue de liste */
/*******************************/
void ajout_en_queue(struct liste_ligneS *liste, struct ligneS *new);


/* Insertion d'une même ligne */
/******************************/
void ajout_avec_decalage(struct fichier *file, int fdClient, struct ligneS *new);


/* Traitement d'une MàJ de buffer d'une ligne existante dans le fichier file */
/*****************************************************************************/
void remplir_ligne(struct ligneS *cible, int num, int taille,char *buffer);


/* Traitement d'une sauvegarde d'une ligne existante dans le fichier file */
/**************************************************************************/
void sauvegarder_ligne_existante(struct liste_ligneS *listemodif,
		 			struct liste_ligneS *liste, int num, int taille, char *buffer);

/* Enregistrement en temps réelle dans la liste de ligne */
/*********************************************************/
void sauvegarde_temporaire(struct liste_ligneS *liste, int num,
														int taille, char *buffer);


/* Remplir le cache pour garder une copie de la ligne avant une modif par l'user */
/*********************************************************************************/
void remplir_cache(struct liste_ligneS *liste, struct client *clt, int num);

/* Free de la struct char et de la struct ligne */
/************************************************/
void detruire(struct ligneS *ligne);


/* Suppression d'une ligne du tableau file->tabLigne */
/*****************************************************/
void supprimer_ligne(struct ligneS *ligne);


/* Suppression d'une ligne des 2 listes chainées de ligne */
/**********************************************************/
void suppriession_dans_deux_listes(struct fichier *file, int fdClient, int num_ligne);


/* Suppression de toutes les lignes de la liste et destruction de la structure */
/*******************************************************************************/
void destruction_liste_ligne(struct liste_ligneS *liste);


/* Verification si liste contient la ligne de numero num */
/*********************************************************/
int contains_ligne(struct liste_ligneS *liste, int num);


/* Décallage de tous les numeros de ligne  de 1 à partir de ligne */
/******************************************************************/
void decalage_num(struct fichier *file, int fdClient,struct ligneS *ligne, int decalage);


/* Fonction est invoquée dans decalage_num, elle permet de verfier
   si les numeros de ligne qu'on decale dans ListeLigne sont presentes
   dans ListeLigneModif pour les decaler aussi ! Elle permet aussi de
   mettre à jour la ligne que l'utilisateur est entrain d'editer */
/*****************************************************************/
void verif_liste_ligne_modif(struct liste_ligneS *liste,
											 int num, int decalage);


/* Affichage de tous les champs de la ligne passée en paramètre */
/****************************************************************/
void display_ligne(struct ligneS *ligne);


/* Affichage de toutes les lignes de la liste de ligne */
/*******************************************************/
void display_liste(struct liste_ligneS *liste);

#endif