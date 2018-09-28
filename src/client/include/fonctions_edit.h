#ifndef FONCTIONS_EDIT_H
#define FONCTIONS_EDIT_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "editeur_gtk.h"
#include "structures.h"
#include "fonctions_open.h"
#include "fonctions_ligne.h"


/* Evenement clavier faite par l'utilisateur */
/*********************************************/
void cb_edit(GtkWidget *p_widget,  GdkEventKey *event, gpointer data);


/* Evenement curseur : pour avoir le numero de ligne est le numero de colonne */
/******************************************************************************/
void update_statusbar(GtkTextBuffer *buffer, GtkStatusbar  *statusbar);


/* Evenement curseur */
/*********************/
void mark_set_callback(GtkTextBuffer *buffer, const GtkTextIter *new_location, 
		GtkTextMark *mark, gpointer data);


#endif
