#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <signal.h>
#include <pthread.h>

#define NBFILE 3


struct ligneS{
	int num;
	int taille;
	char *buffer;
	struct ligneS *next;
	struct ligneS *prev;
};

struct liste_ligneS{
	struct ligneS tabLigne;
	pthread_mutex_t mutexListeLigne;
};

struct commande {
	char *commandeName;
	struct ligneS ligne;
	int rempli;
};

struct broadcast{
	int fd_client;
	char *fileName;
	struct commande commandeBR;
	pthread_cond_t condCommandeBR;
	pthread_mutex_t mutexCommandeBR;
};

struct client{

	char *fileName;
	char *commandeName_Em;
	char *commandeName_Recep;
	char *buffer_Em;
	pthread_t thread_reception;
	pthread_t thread_emission;
	
	int en_cours_dedition;

	int client_deconnecte;
	int client_en_vie;
	int client_en_panne;
	int fdSockEmission;
	int fdSockReception;

	char *cache;
	
	struct client *next;
	struct client *prev;
	
	struct commande commandeInfo;
	pthread_cond_t condCommande;
	pthread_mutex_t mutexCommande;	
};

struct fichier{
	
	char *name;	/* Nom du fichier */

	struct client fdSocks;	/* Liste vide (element fantôme) */
	pthread_mutex_t mutexTabFdSock;	/* Pour acceder au tableau fdSock et aux */
									/* variables en exclusion mutuelle */
	struct liste_ligneS listeLigne;	/* liste qui contient toutes les lignes du fichier*/
	struct liste_ligneS listeLigneModif;	/* liste qui contient toutes les lignes */
											/* du fichier qui sont entrain d'etre modfier */
};

struct liste_fichier{
	
	int taille;
	struct fichier liste[NBFILE];
	
	struct client listeClientTmp;
	pthread_mutex_t mutex;
	
	struct broadcast struct_br;
};


/* Pointeur d'entrée */
/*********************/
struct liste_fichier liste_file;

#endif
