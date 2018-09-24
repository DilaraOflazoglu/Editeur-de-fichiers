#include "fonctions_fichier.h"

static char *bufferFichier;
static int nb_char=0;
static int num_ligne = 1;


//OK
/* Création et initialisation d'un nouveau pointeur de fichier */
/***************************************************************/
void initialiser_fichier(struct liste_fichier *tab_file, char *name){

    char *fileName;

    int taille = strlen(name) + strlen(".txt") + 1;
    fileName = initialiser_buffer(taille);

    snprintf(fileName, taille, "%s.txt", name);

    tab_file->liste[tab_file->taille].name = initialiser_buffer(taille);
    strncpy(tab_file->liste[tab_file->taille].name, fileName, taille);

    tab_file->liste[tab_file->taille].fdSocks.next = &tab_file->liste[tab_file->taille].fdSocks;
    tab_file->liste[tab_file->taille].fdSocks.prev =  &tab_file->liste[tab_file->taille].fdSocks;

    pthread_mutex_init(&tab_file->liste[tab_file->taille].mutexTabFdSock, NULL);

/* Initialisation des liste_ligneS : listeLigne et listeLigneModif */
    pthread_mutex_init(&tab_file->liste[tab_file->taille].listeLigne.mutexListeLigne, NULL);
    pthread_mutex_init(&tab_file->liste[tab_file->taille].listeLigneModif.mutexListeLigne, NULL);

    tab_file->liste[tab_file->taille].listeLigne.tabLigne.num = 0;
    tab_file->liste[tab_file->taille].listeLigneModif.tabLigne.num = 0;

    tab_file->liste[tab_file->taille].listeLigne.tabLigne.next = &tab_file->liste[tab_file->taille].listeLigne.tabLigne;
    tab_file->liste[tab_file->taille].listeLigneModif.tabLigne.next = &tab_file->liste[tab_file->taille].listeLigneModif.tabLigne;

    tab_file->liste[tab_file->taille].listeLigne.tabLigne.prev = &tab_file->liste[tab_file->taille].listeLigne.tabLigne;
    tab_file->liste[tab_file->taille].listeLigneModif.tabLigne.prev = &tab_file->liste[tab_file->taille].listeLigneModif.tabLigne;

    tab_file->taille++;

    free(fileName);
}



void sauvegarder_modif_save_fichier(struct fichier * file){
    int fdFile;
    struct ligneS * cible;

    remove(file->name);

    if((fdFile=open(file->name, O_CREAT | O_RDWR, 0666))==-1){
        printf("fonctions_fichier : Probleme open fdFile");  exit(-1);
    }

/* Remplire le fichier */
    pthread_mutex_lock(&file->listeLigne.mutexListeLigne);
    cible=file->listeLigne.tabLigne.next;

    while(cible != &file->listeLigne.tabLigne){

        if(write(fdFile, cible->buffer, cible->taille) == -1){
            perror("fonctions_fichier : Probleme write"); exit(-1);
        }

        if(write(fdFile, "\n", 1) ==-1){
            perror("fonctions_fichier : Probleme write"); exit(-1);
        }
        cible = cible->next;
    }

    close(fdFile);
    pthread_mutex_unlock(&file->listeLigne.mutexListeLigne);
}


void initialiser_de_tous_les_fichiers(){

    /* Initialisation commande pour le broadcast */
    pthread_mutex_init(&liste_file.struct_br.mutexCommandeBR, NULL);
    pthread_cond_init(&liste_file.struct_br.condCommandeBR, NULL);
    liste_file.struct_br.commandeBR.rempli=0;

    liste_file.struct_br.commandeBR.commandeName = initialiser_buffer(6);

    liste_file.taille = 0;

    liste_file.listeClientTmp.next = &liste_file.listeClientTmp;
    liste_file.listeClientTmp.prev = &liste_file.listeClientTmp;
    pthread_mutex_init(&liste_file.mutex, NULL);

/* Ajouter tous les fichiers existants  */
    initialiser_fichier(&liste_file, "histoire");
    bufferiser_fichier("histoire");

    initialiser_fichier(&liste_file, "data_privacy");
    bufferiser_fichier("data_privacy");

    initialiser_fichier(&liste_file, "technology");
    bufferiser_fichier("technology");
}


//OK
/* Cette fonction permet de lire dans un fichier quelconque_bis.txt de
    le redimensionner et le copier dans un fichier quelconque.txt créé */
/***********************************************************************/
void bufferiser_fichier(char *name){

    int fdFileReel, fdFileCopie, taille;
    char *buffer, *fileNameCopie, *fileNameReel;
    struct fichier *file;

    taille = strlen(name) + strlen("./fichier/  .txt");
    fileNameReel = initialiser_buffer(taille);

    memset(fileNameReel, 0, taille);
    strcat(fileNameReel, name);
    strcat(fileNameReel, ".txt");

    taille = strlen(name) + strlen("./fichier/_bis.txt");
    fileNameCopie = initialiser_buffer(taille);

    memset(fileNameCopie, 0, taille);
    strcat(fileNameCopie, "./fichier/");
    strcat(fileNameCopie, name);
    strcat(fileNameCopie, "_bis.txt");

    file = trouver_Fichier(fileNameReel);

    memset(fileNameReel, 0, strlen(fileNameReel));
    strcat(fileNameReel, "./fichier/");
    strcat(fileNameReel, name);
    strcat(fileNameReel, ".txt");

    buffer = initialiser_buffer(2047);

    bufferFichier = initialiser_buffer(79);

/* Celui dans lequel on ecrit */
    if((fdFileReel=open(fileNameReel, O_CREAT | O_RDWR, 0666))==-1){
        perror("fonctions_fichier : Probleme open fdFileReel");
        goto error_open_fdFileReel;
    }

/* Celui dans lequel on lit */
    if((fdFileCopie=open(fileNameCopie, O_RDONLY, 0666))==-1){
        perror("fonctions_fichier : Probleme open fdFileCopie");
        goto error_open_fdFileCopie;
    }

    while(read(fdFileCopie, buffer, 2047)>0){
        redimensionner_ligne_fichier(fdFileReel, buffer, file);
        memset(buffer, 0, 2047);
    }

    fin_dimension_ligne_fichier(fdFileReel, file);

    free(buffer);
    free(bufferFichier);
    free(fileNameCopie);
    free(fileNameReel);

    close(fdFileCopie);
    close(fdFileReel);
    printf("Fichier bufferisé ! \n");
    return;

error_open_fdFileCopie :
    close(fdFileReel);

error_open_fdFileReel :
    free(bufferFichier);
    free(buffer);
    free(fileNameCopie);
    free(fileNameReel);
    exit(-1);
}


//OK
/* Recuperer un fichier et le dimensionner a 80 caracteres par ligne */
/*********************************************************************/
void redimensionner_ligne_fichier(int fd, char *buffer, struct fichier *file){

    char *buf, *k, *t, *token_last, *token_previous;
    const char schlash[2] = "\n", espace[2] = " ";
    struct ligneS *new;

    buf = initialiser_buffer(2050);

    k = strtok(buffer, schlash);

/* On enleve tous les '\n' du fichier */
    while(k != NULL){
        strcat(buf, k);
        k = strtok(NULL, schlash);
    }

/* On enleve tous les " " du buffer et création de buffer de 80 carateres maximum */
    t = strtok(buf, espace);
    token_last = strdup(t);
    t = strtok(NULL, espace);

    while(t != NULL){
        token_previous = strdup(token_last);
        token_last = strdup(t);

        if((nb_char+(((int)strlen(token_previous)) + 1)) > 79){

            if(write(fd, bufferFichier, strlen(bufferFichier)) ==- 1){
                perror("fonctions_fichier : Probleme write bufferFichier"); exit(-1);
            }
            if(write(fd, "\n", 1) ==-1){
                perror("fonctions_fichier : Probleme write '\n'"); exit(-1);
            }

            new = nouvelle_ligne(num_ligne, strlen(bufferFichier), bufferFichier);
            ajouter_ligne(file, 0, new);
            num_ligne++;

            memset(bufferFichier, 0, 79);
            nb_char = 0;
        }

        strcat(bufferFichier, token_previous);
        strcat(bufferFichier, " ");
        nb_char = strlen(bufferFichier);

        t = strtok(NULL, espace);
    }

    if(nb_char + strlen(token_last) > 79){
        if(write(fd, bufferFichier, strlen(bufferFichier)) ==- 1){
            perror("fonctions_fichier : Probleme write bufferFichier"); exit(-1);
        }
        if(write(fd, "\n", 1) ==-1){
            perror("fonctions_fichier : Probleme write '\n'"); exit(-1);
        }

        new = nouvelle_ligne(num_ligne, strlen(bufferFichier), bufferFichier);
        ajouter_ligne(file, 0, new);
        num_ligne++;

        memset(bufferFichier, 0, 79);
        nb_char = 0;
    }

    strcat(bufferFichier, token_last);
    strcat(bufferFichier, " ");
    nb_char+=strlen(token_last);

    free(buf);
}


//OK
/* Cette fonction est utilisée juste apres la fonction redimensionner_ligne_fichier
    car redimensionner_ligne_fichier met les restes des 2047%80 caraceteres
      redimensionner dans une variable global, cette fonction permet de
        terminer le travail de la fonction redimensionner_ligne_fichier */
/**********************************************************************************/
void fin_dimension_ligne_fichier(int fd, struct fichier *file){

/* Envoie de la fin du fichier */
    if(write(fd, bufferFichier, strlen(bufferFichier)) == -1){
        perror("fonctions_fichier : Probleme write bufferFichier"); exit(-1);
    }

    struct ligneS *new = nouvelle_ligne(num_ligne, strlen(bufferFichier),
                                                              bufferFichier);
    ajouter_ligne(file, 0, new);
    memset(bufferFichier, 0, 79);

    nb_char = 0;
    num_ligne = 1;
}


//OK
/* Trouver le fichier correspondant au nom donnée */
/**************************************************/
struct fichier *trouver_Fichier(char *name){
	int i;
	for(i=0; i<liste_file.taille; i++){
		if(strcmp(&liste_file.liste[i].name[0], &name[0]) == 0){
			return &liste_file.liste[i];
		}
	}
	return NULL;
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