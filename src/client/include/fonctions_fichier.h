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


struct fichier *initialiserFichier();

void detruire_fichier();

void remove_directory();

char *initialiser_buffer(int taille);

#endif