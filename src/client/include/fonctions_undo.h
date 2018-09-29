#ifndef FONCTIONS_UNDO_H
#define FONCTIONS_UNDO_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "editeur_gtk.h"


/* Evenement du click sur le bouton d'annulation d'édition */
/***********************************************************/
void cb_undo(GtkWidget *p_widget, gpointer pData);


#endif