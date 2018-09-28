#ifndef FONCTIONS_THREAD_BROADCAST_H
#define FONCTIONS_THREAD_BROADCAST_H

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


/* FONCTION PRINCIPAL DU THREAD BROADCAST */
/******************************************/
void *broadcast(void *arg);


/* FONCTION UTILISÉ PAR LE THREAD BROADCAST */
/********************************************/
void broadcast_info_All(struct fichier *file, char *envoieMess);

void broadcast_info_without(struct fichier *file, int fdClient,
												char *envoieMes);

void commande_edit_BR(struct fichier *file, int fdClient, int num_ligne);

void commande_annul_BR(struct fichier *file, int fdClient, int num_ligne);

void commande_insert_BR(struct fichier *file, int fdClient, int num_ligne);

void commande_save_BR(struct fichier *file, int fdClient, 
							int num_ligne, char *buffer, int taille, int tmp);

void sauvegarde_taille_zero(struct fichier *file, int num_ligne, int tmp);

void redimensionner_ligne_BR(struct fichier *file,  int fdClient, int ligne, char *buffer);

void preparation_broadcast_save(struct fichier *file, int fdClient, int num_ligne, 
										int tailleBuffer, char *buffer, int first);

void commande_delete_BR(struct fichier *file, int fdClient, int num_ligne);

char *attente_signal_commande(struct liste_fichier *file, int *fdClient,
						int *ligne, int *taille, char *commande, char *fileName);

void signaler_thread_broadcast(struct liste_fichier *file, int fd_client, 
				char *type, int ligne, int taille, char *buffer, char *fileName);



#endif