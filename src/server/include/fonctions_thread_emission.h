#ifndef FONCTIONS_THREAD_EMISSION_H
#define FONCTIONS_THREAD_EMISSION_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include "structures.h"
#include "fonctions_ligne.h"
#include "fonctions_client.h"
#include "fonctions_fichier.h"
#include "fonctions_thread_broadcast.h"


/* FONCTION PRINCIPAL DU THREAD EMISSION */
/*****************************************/
void *emission(void *arg);


/* FONCTIONS UTILISÉES PAR LE THREAD RECEPTION */
/***********************************************/
char *attente_reception_commande(struct client *clt, int *ligne, 
					int *taille, char *commande);

void envoie_noms_fichier(struct client *clt);
void envoie_nom_dun_fichier(struct client *clt, char *name);

void envoie_fichier(struct fichier *file, struct client *clt);

void commande_close_Em(struct fichier *file, struct client *clt);

void commande_edit_Em(struct fichier *file, struct client *clt, int ligne);

void commande_annul_Em(struct fichier *file, struct client *clt, int ligne);

void client_tombe_en_panne(struct fichier *file, struct client *clt);

#endif