#ifndef EDITEUR_GTK_H
#define EDITEUR_GTK_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <pthread.h>
#include "structures.h"
#include "fonctions_edit.h"
#include "fonctions_undo.h"
#include "fonctions_save.h"
#include "fonctions_open.h"
#include "fonctions_close.h"
#include "fonctions_connection.h"


int fdSockServer;	


/* Fonction principale de GTK */
/******************************/
void mainGTK(int fdServer, int argc, char* argv []);


/* Fonction qui s'éxecute toutes les 20 secondes, elle verifie l'état de
l'application pour voir si tout est en ordre, si le thread crée est en vie */
/***************************************************************************/
gboolean message_en_vie(gpointer p_text_view);

#endif
