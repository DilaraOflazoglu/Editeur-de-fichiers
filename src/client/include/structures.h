#ifndef STRUCTURES_H
#define STRUCTURES_H

#define SIZELINE 80
#define NBFILE 1

struct ligneC{
	int num;
	int taille;
	char *buffer;
	struct ligneC *next;
	struct ligneC *prev;
};

struct liste_ligneC{
	struct ligneC tabLigne;
	pthread_mutex_t mutexListeLigne;
};

struct fichier{
	
	char *fileName;
	int fileName_rempli;
	pthread_cond_t condfileName;
	pthread_mutex_t mutexfileName;

	struct liste_ligneC listeLigne;	/* liste qui contient toutes les lignes du fichier*/
	struct liste_ligneC listeLigneModif;	/* liste qui contient toutes les lignes */
											/* du fichier qui sont entrain d'etre modfier */
	pthread_mutex_t mutexVariablesGTK;
	int updated;
	int file_open;	/* Pour bloquer a une ouverture de fichier */
   	int widget_open;
   	int en_cours_de_sauvegarde;

   	int en_cours_dedition;
	int end_offset;

	int attente_de_reponse;
	int nb_non_reponse;

	int fdSockReception;
	int fdSockEmission;
	pthread_t thread_reception;
};


/* Pointeur d'entrée */
/*********************/
struct fichier *fichierInfo;

#endif
