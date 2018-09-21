#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "structures.h"
#include "fonctions_fichier.h"
#include "fonctions_connection.h"



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


    fermeture_socket_tcp(fichierInfo->fdSockReception);
    fermeture_socket_tcp(fichierInfo->fdSockEmission);
    detruire_fichier();

    if(fichierInfo->file_open == 0)
        remove_directory();

	return EXIT_SUCCESS;
}
