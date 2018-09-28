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


/* À Utiliser pour les Commandes SAVEF et DELET */
/* Envoie d'une mise à jour à tous les utilisateurs */
/****************************************************/
void broadcast_info_All(struct fichier *file, char *envoieMess);


/* À Utiliser pour les Commandes EDITF et ANNUL */
/* Envoie d'une MàJ à tous les utilisateurs sauf à celui qui l'a déclenché */
/***************************************************************************/
void broadcast_info_without(struct fichier *file, int fdClient,
												char *envoieMes);


/* Preparation du message d'une nouvelle edition a tous les utilisateurs */
/*************************************************************************/
void commande_edit_BR(struct fichier *file, int fdClient, int num_ligne);


/* Preparation du message d'une annulation d'edition a tous les utilisateurs */
/*****************************************************************************/
void commande_annul_BR(struct fichier *file, int fdClient, int num_ligne);


/* Insertion de ligne */
/**********************/
void commande_insert_BR(struct fichier *file, int fdClient, int num_ligne);


/* Preparation du message d'une sauvegarde d'edition a tous les utilisateurs */
/*****************************************************************************/
void commande_save_BR(struct fichier *file, int fdClient, 
							int num_ligne, char *buffer, int taille, int tmp);


/* Envoie d'un message SAVE --> Modification du contenu de la ligne sans ajout */
/*******************************************************************************/
void sauvegarde_taille_zero(struct fichier *file, int num_ligne, int tmp);


/* Attention !! Cette methode n'est utilisée que dans commande_save_BR et
 verrou de "listeLigneModif" est pris avant l'invocation de cette methode */
/* Redimensionner le buffer afin de créer des lignes de 80 caracteres maximum */
/******************************************************************************/
void redimensionner_ligne_BR(struct fichier *file,  int fdClient, int ligne, char *buffer);


/* Attention !! Cette methode n'est utilisée que dans redimensionner_ligne_BR
 	et le verrou de "listeLigneModif" est pris avant l'invocation de la
 	methode redimensionner_ligne_BR */
/* Création et ajout de ligne dans la liste et complétion du message d'envoie */
/******************************************************************************/
void preparation_broadcast_save(struct fichier *file, int fdClient, int num_ligne, 
										int tailleBuffer, char *buffer, int first);


/* Preparation du message d'une suppression de ligne a tous les utilisateurs */
/*****************************************************************************/
void commande_delete_BR(struct fichier *file, int fdClient, int num_ligne);


/* On remplie la struct commande du thread Broadcast pour lui signaler une commande */
/************************************************************************************/
char *attente_signal_commande(struct liste_fichier *file, int *fdClient,
						int *ligne, int *taille, char *commande, char *fileName);


/* Fonction utiliser par le thread Emission et Reception
pour signaler une nouvelle commande au thread BROADCASt */
/********************************************************/
void signaler_thread_broadcast(struct liste_fichier *file, int fd_client, 
				char *type, int ligne, int taille, char *buffer, char *fileName);



#endif
