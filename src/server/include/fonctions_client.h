#ifndef FONCTIONS_CLIENT_H
#define FONCTIONS_CLIENT_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "stddef.h"
#include "structures.h"
#include "fonctions_ligne.h"
#include "fonctions_connection.h"


/* Création et initialisation d'un nouveau pointeur de client */
/**************************************************************/
struct client *nouveau_client(int fdEmission, int fdReception);


/* Ajouter un client à la liste chainé */
/***************************************/
void ajouter_client_dans_fichier(struct fichier *file, struct client *new_clt);/* Ajouter un client au tableau */


/* Ajouter un client à la liste chainé temporaire */
/**************************************************/
void ajouter_client_liste_temporaire(struct client *new_clt);


/* Le champs clientNonConcerne  represente le client qui a fait la modif  */
/* On décale le champs en_cours_dedition de tous les utlisateurs sauf lui */
/**************************************************************************/
void verif_en_cours_dedition_client(struct fichier *file,  int clientNonConcerne, 
												int num, int decalage);


/* Supprimer un client du tableau */
/**********************************/
void supprimer_client(struct client *clt);	/* Supprimer un client du tableau */


/* Ajouter un client à la liste chainé temporaire */
/**************************************************/
void supprimer_client_liste_temporaire(struct client *new_clt);


/* Free la mémoire du pointeur d'un client */
/*******************************************/
void detruire_client(struct client *clt);


/* Affichage de tous les clients */
/*********************************/
void display(struct fichier *file);		/* Affichage de structure client du fichier */

#endif