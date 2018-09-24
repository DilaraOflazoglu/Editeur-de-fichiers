#ifndef FONCTIONS_THREAD_RECEPTION_H
#define FONCTIONS_THREAD_RECEPTION_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include "fonctions_client.h"
#include "fonctions_connection.h"
#include "fonctions_fichier.h"
#include "fonctions_thread_broadcast.h"


/* FONCTION PRINCIPAL DU THREAD RECEPTION */
/******************************************/
void *reception(void *arg);


/* FONCTION UTILISÉ PAR LE THREAD RECEPTION */
/********************************************/
void signal_commande_emission(struct client *clt, char *type, int ligne, 
								int taille, char *buffer);

struct fichier *commande_open_RECEP(struct client *clt);

void commande_edit_RECEP(struct client *clt);

void commande_insert_RECEP(struct client *clt);

void commande_annul_RECEP(struct fichier *file, struct client *clt);

void commande_alive_RECEP(struct fichier *file, struct client *clt);

void commande_save_RECEP(struct fichier *file, struct client *clt, int tmp);

void commande_delete_RECEP(struct fichier *file, struct client *clt);


/* Chaque  envoie d'entier par le serveur se termine  par un underscore (_) */
/* Cette Fonction permet d'extraire l'entier du message reçu par le Serveur */
/****************************************************************************/
int reception_entier(int fdSockClientReception);

#endif
