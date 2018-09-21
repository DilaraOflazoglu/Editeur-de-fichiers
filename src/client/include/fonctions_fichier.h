#ifndef FONCTIONS_FICHIER_H
#define FONCTIONS_FICHIER_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "structures.h"
#include "fonctions_ligne.h"


/* Création et Allocation memoire d'une structure fichier */
/**********************************************************/
struct fichier *initialiserFichier();


/* Destruction et Désallocation memoire d'une structure fichier */
/****************************************************************/
void detruire_fichier();


/* Suppression du repertoire "fichier" créé avec le fichier demandé */
/********************************************************************/
void remove_directory();


/* Retour un pointeur de chaine de caractere entierement initialisé a zero */
/***************************************************************************/
char *initialiser_buffer(int taille);

#endif