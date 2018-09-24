#ifndef FONCTIONS_RECEPTION_H
#define FONCTIONS_RECEPTION_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include "structures.h"
#include "editeur_gtk.h"
#include "fonctions_ligne.h"
#include "fonctions_connection.h"


/* Fonction Thread reception */		
/****************************/
void *reception_reponse(void *arg);

void reception_nom_fichiers(int fdServer);
void reception_fichier(int fdSockServer);

int reception_entier(int fdSockServer);

void reception_insert(struct fichier *file, int fdSockServer);

void reception_edit(struct fichier *file, int fdSockServer);
void reception_cancel(struct liste_ligneC *liste, int fdSockServer);
void reception_delete(struct liste_ligneC *listemodif,
						struct liste_ligneC *liste, int fdSockServer);
void reception_modif(struct fichier *file, int fdSockServer);
void reception_broadcast(struct fichier *file, int fdSockServer, int tmp);

#endif
