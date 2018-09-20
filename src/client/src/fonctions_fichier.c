#include "fonctions_fichier.h"


//OK
/* Création et Allocation memoire d'une structure fichier */
/**********************************************************/
struct fichier *initialiserFichier(){

	struct fichier *f = (struct fichier *)malloc(sizeof(struct fichier));

	pthread_cond_init(&f->condfileName, NULL);
	pthread_mutex_init(&f->mutexfileName, NULL);
	f->fileName_rempli=0;

/* Initialisation des liste_ligneS : listeLigne et listeLigneModif */
	pthread_mutex_init(&f->listeLigne.mutexListeLigne, NULL);
	pthread_mutex_init(&f->listeLigneModif.mutexListeLigne, NULL);

    f->listeLigne.tabLigne.num = 0;
    f->listeLigneModif.tabLigne.num = 0;

    f->listeLigne.tabLigne.next = &f->listeLigne.tabLigne;
    f->listeLigneModif.tabLigne.next = &f->listeLigneModif.tabLigne;

    f->listeLigne.tabLigne.prev = &f->listeLigne.tabLigne;
    f->listeLigneModif.tabLigne.prev = &f->listeLigneModif.tabLigne;

/* Variable du thread qui gere les evenements de GTK */
    pthread_mutex_init(&f->mutexVariablesGTK, NULL);

    f->updated = 0;
    f->file_open = 0;
   	f->widget_open = 0;
   	f->en_cours_dedition = 0;
	f->end_offset = 0;
    f->attente_de_reponse = 0;
    f->nb_non_reponse = 0;

	return f;
}


/* Destruction et Désallocation memoire d'une structure fichier */
/****************************************************************/
void detruire_fichier(){

	pthread_cond_destroy(&fichierInfo->condfileName);
	pthread_mutex_destroy(&fichierInfo->mutexfileName);

	destruction_liste_ligne(&fichierInfo->listeLigne);
	destruction_liste_ligne(&fichierInfo->listeLigneModif);

    pthread_mutex_destroy(&fichierInfo->mutexVariablesGTK);

	free(fichierInfo);
}


//OK
/* suppression du repertoire "fichier" créé avec le fichier demandé */
/********************************************************************/
void remove_directory(){

	DIR *directory;           /* pointeur de répertoire */
    struct dirent *entry;     /* représente une entrée dans un répertoire. */
    struct stat file_stat;    /* informations sur un fichier. */
    char *buffer;
    int taille;

/* On ouvre le dossier. */
    if((directory= opendir("fichier")) == NULL ){
        perror("Thread emetteur : Probleme opendir(fichier)"); exit(-1);
    }
/* On boucle sur les entrées du dossier. */
    while ((entry = readdir(directory)) != NULL ) {

    /* On "saute" les répertoires "." et "..". */
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 ){
            continue;
        }
		taille = strlen("fichier/") + strlen(entry->d_name) + 1; /* repertoire / file */


    	if((buffer = (char *)malloc(taille*sizeof(char))) == NULL){
    		perror("Thread emetteur : Probleme malloc buffer"); exit(-1);
    	}

    	memset(buffer, 0, taille);
        snprintf(buffer, taille, "fichier/%s", entry->d_name);

    /* On récupère des infos sur le fichier. */
        if(stat(buffer, &file_stat) == -1){
        	 perror("Thread emetteur : Probleme fonction stat"); exit(-1);
        }
    /* On est sur un fichier, on le supprime. */
        if(S_ISREG(file_stat.st_mode)) {

            remove(buffer);
        }
        free(buffer);
    }
/* On ferme le dossier. */
    if(closedir(directory) == -1){
    	 perror("Thread emetteur : Probleme fonction closedir"); exit(-1);
    }

    if(remove("fichier") == -1){
    	perror("Thread emetteur : Probleme fonction remove"); exit(-1);
	}
}


char *initialiser_buffer(int taille){

    int i;
    char *buffer;

    buffer = (char *)malloc(taille*sizeof(char));

    if(buffer == NULL){
        perror("fonctions_fichier : PB malloc buffer"); exit(-1);
    }
    memset(buffer, 0, taille);

    for(i = 0; i<taille; i++){
        buffer[i] = 0;
    }

    return buffer;
}