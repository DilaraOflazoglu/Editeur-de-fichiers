#ifndef FONCTIONS_SAVE_H
#define FONCTIONS_SAVE_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "editeur_gtk.h"


/* Evenement du click sur le bouton d'enregistrement */
/*****************************************************/
void cb_save(GtkWidget *p_widget, gpointer pData);


/* Cette fonction sert a recuperer la ligne entiere editer par l'utilisateur 
le param tmp sert a dissocier le vrai SAVEF CàD quand l'utilisateur appuie sur
le bouton save, des SAVEF envoyer automatiquement tous les 3 carateres ecrit*/
/*****************************************************************************/
char *recupere_donnees_save(gpointer pData);


/* Cette fonction sert a envoyer une requerte SAVEF  de la ligne recuperer au serveur, 
	le param tmp sert a dissocier le vrai SAVEF CàD quand l'utilisateur appuie sur le 
	bouton save, des SAVEF envoyer automatiquement tous les 3 carateres ecrit  */
/************************************************************************************/
void envoyer_save(char *buffer, int ligne);


/* Met à jour le buffer de GTK quand on recoit un BROADCAST 2 cas sont possible :
	-	Utilisateur est en cours d'édition --> on recupere ses données on les met
		dans la liste de ligne et on affiche
	-	l'utilisateur n'est pas en cours d'édition --> alors on affiche la liste
		de lignes
*/
/********************************************************************************/
gboolean cb_update(gpointer p_text_view);


/* Cette fonction sert a envoyer une requerte SAVEF  de la ligne recuperer au serveur,
	le param tmp sert a dissocier le vrai SAVEF CàD quand l'utilisateur appuie sur le
	bouton save, des SAVEF envoyer automatiquement tous les 3 carateres ecrit  */
/************************************************************************************/
void envoyer_save_temporaire(char *buffer, int ligne);


#endif
