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


/* Fonction principal du thread RECEPTEUR */
/******************************************/
void *reception_reponse(void *arg);


/* Reception des noms de fichiers pour que l'user pense les avoir localement */
/*****************************************************************************/
void reception_nom_fichiers(int fdServer);


/* Reception du fichier et création de la liste de ligne */
/*********************************************************/
void reception_fichier(int fdSockServer);


/* Chaque  envoie d'entier par le serveur se termine  par un underscore (_) */
/* Cette Fonction permet d'extraire l'entier du message reçu par le Serveur */
/****************************************************************************/
int reception_entier(int fdSockServer);


/* Insertion de ligne */
/**********************/
void reception_insert(struct fichier *file, int fdSockServer);


/* Reception de la réponse de la demande d'edition faite par l'utilisateur */
/***************************************************************************/
void reception_edit(struct fichier *file, int fdSockServer);


/* Reception d'une annulation d'edtiion d'un Utilsateur */
/********************************************************/
void reception_cancel(struct liste_ligneC *liste, int fdSockServer);


/* Reception d'une suppression de ligne d'un Utilisateur */
/*********************************************************/
void reception_delete(struct liste_ligneC *listemodif,
						struct liste_ligneC *liste, int fdSockServer);


/* Reception d'edition de ligne d'un autre User */
/************************************************/
void reception_modif(struct fichier *file, int fdSockServer);


/* Reception d'une sauvegarde d'une ligne */
/******************************************/
void reception_broadcast(struct fichier *file, int fdSockServer, int tmp);

#endif
