#ifndef FONCTIONS_CLOSE_H
#define FONCTIONS_CLOSE_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include "editeur_gtk.h"


/*evenement: fermeture GTK */
/***************************/
void cb_close_croix(GtkWidget *p_widget, gpointer *user_data);

/*evenement: fermeture GTK */
/***************************/
void cb_close_bouton(GtkWidget *p_window, gpointer *user_data);


/* Si on est en cours d'édition, on envoie un message au thread recepteur
du serveur pour dire qu'on annule la demande d'edition faite auparavant */
/************************************************************************/
void quitter_sans_enregistrer();


/* Si on est en cours d'édition, on envoie un message au thread recepteur
	du serveur pour dire qu'on fait une sauvegarde de nos modifications */
/************************************************************************/
void enregistrer_quitter(GtkWidget *p_window, gpointer *user_data);


/* On envoie un message au thread recepteur
du serveur pour dire qu'on quitte l'application */
/************************************************/
void quitter();

#endif
