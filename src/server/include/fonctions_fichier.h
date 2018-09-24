#ifndef FONCTIONS_FICHIER_H
#define FONCTIONS_FICHIER_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include "structures.h"
#include "fonctions_ligne.h"

void initialiser_fichier(struct liste_fichier *tab_file, char *name); /* Creer et Initialiser variable fichier */

void bufferiser_fichier(char *name);

void redimensionner_ligne_fichier(int fd, char *buffer, struct fichier *file);

void fin_dimension_ligne_fichier(int fd, struct fichier *file);

struct fichier *trouver_Fichier(char *name);

void sauvegarder_modif_save_fichier(struct fichier * file);

void initialiser_de_tous_les_fichiers();

char *initialiser_buffer(int taille);

#endif
