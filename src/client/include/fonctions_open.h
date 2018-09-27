#ifndef FONCTIONS_OPEN_H
#define FONCTIONS_OPEN_H


#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include "structures.h"
#include "editeur_gtk.h"
#include "fonctions_fichier.h"


/*evenement: ouverture fichier*/
/******************************/
void cb_open(GtkWidget *p_widget, gpointer user_data);


/*procedure d'ouverture de fichier*/
/**********************************/
void ouvrir_fichier(GtkWidget *bouton, GtkWidget *file_selection);


/* Extraction du nom du fichier a partir chemin et
initialisation du nom du fichier  de la structure commande */
/***********************************************************/
void remplir_fileName(char *chemin);


/* Affichage des lignes de la structure liste ligne dans le buffer de GTK */
/**************************************************************************/
void remplir_buffer(GtkTextBuffer *buffer);


/* Lors d'une MàJ du buffer on remet le curseur a la place avant la MàJ */
/************************************************************************/
void replacer_curseur(GtkTextBuffer *buffer, int ligne, int colonne);

#endif
