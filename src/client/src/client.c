#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "structures.h"
#include "fonctions_fichier.h"
#include "fonctions_connection.h"
#include "fonctions_reception.h"
#include "fonctions_close.h"
#include "editeur_gtk.h"



/* Catch Signal Handler functio */
 void signal_callback_handler(int signum){

     printf("Caught signal SIGPIPE %d\n", signum);
}

int main (int argc, char* argv []) {

  	if(argc != 4){
        perror("Main : Probleme sur le nombre d'argument !\n"); exit(-1);
    }

	signal(SIGPIPE, signal_callback_handler);

	fichierInfo = initialiserFichier();

	fichierInfo->fdSockReception = connection(argv[1], argv[2]);
	fichierInfo->fdSockEmission = connection(argv[1], argv[3]);

	if(pthread_create(&fichierInfo->thread_reception, NULL, reception_reponse,
		&fichierInfo->fdSockReception) != 0){
		perror("Main : pb invocation pthread_create \n"); 	exit(EXIT_FAILURE);
	}

   	mainGTK(fichierInfo->fdSockEmission, argc, argv);
   	printf("Main : fonction maniGTK terminée ! \n");

	if(pthread_join(fichierInfo->thread_reception, NULL) != 0){
		perror("Main : pb invocation pthread_join \n"); exit(EXIT_FAILURE);
	}

    printf("Main : Thread_reception terminé !\n");

    fermeture_socket_tcp(fichierInfo->fdSockEmission);
    detruire_fichier();

    if(fichierInfo->file_open == 0)
        remove_directory();

	return EXIT_SUCCESS;
}