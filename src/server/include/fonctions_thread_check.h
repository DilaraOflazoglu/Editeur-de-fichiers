#ifndef FONCTIONS_THREAD_CHECK_H
#define FONCTIONS_THREAD_CHECK_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include "fonctions_client.h"
#include "structures.h"


/* Parcours de chaque fichier puis attente de "15 secondes" */
/************************************************************/
void *check();


/* Parcours de liste chainée pour supprimer les clients déconnectés */
/********************************************************************/
void check_one_file(struct fichier *file);


/* Parcours de liste chainée temporaire pour supprimer les clients déconnectés */
/*******************************************************************************/
void check_liste_temporaire();


/* Suppression d'un client de la liste des clients qui est tombé en panne */
/* Verification que le client n'est pas en cours d'édition sinon on envoie
	un message d'annulation a tous les autres clients */
/**************************************************************************/
void suppression_client_en_panne(struct fichier *file, struct client *clt);

#endif
