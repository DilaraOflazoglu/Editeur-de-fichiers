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


/* Création et initialisation d'un nouveau pointeur de fichier */
/***************************************************************/
void initialiser_fichier(struct liste_fichier *tab_file, char *name); /* Creer et Initialiser variable fichier */


/* Cette fonction permet de lire dans un fichier quelconque_bis.txt de
    le redimensionner et le copier dans un fichier quelconque.txt créé */
/***********************************************************************/
void bufferiser_fichier(char *name);


/* Recuperer un fichier et le dimensionner a 80 caracteres par ligne */
/*********************************************************************/
void redimensionner_ligne_fichier(int fd, char *buffer, struct fichier *file);


/* Cette fonction est utilisée juste apres la fonction redimensionner_ligne_fichier
    car redimensionner_ligne_fichier met les restes des 2047%80 caraceteres
      redimensionner dans une variable global, cette fonction permet de
        terminer le travail de la fonction redimensionner_ligne_fichier */
/**********************************************************************************/
void fin_dimension_ligne_fichier(int fd, struct fichier *file);


/* Trouver le fichier correspondant au nom donnée */
/**************************************************/
struct fichier *trouver_Fichier(char *name);


/* Pour sauvegarder les modifications faites par les clients dans le fichier local du serveur */
/**********************************************************************************************/
void sauvegarder_modif_save_fichier(struct fichier * file);


/* Initialiser les champs de tous les fichiers */
/***********************************************/
void initialiser_de_tous_les_fichiers();


/* Retour un pointeur de chaine de caractere entierement initialisé a zero */
/***************************************************************************/
char *initialiser_buffer(int taille);

#endif
