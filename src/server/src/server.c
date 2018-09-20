#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "structures.h"
#include "fonctions_fichier.h"
#include "fonctions_connection.h"


/* Catch Signal Handler functio */
 void signal_callback_handler(int signum){

     printf("Caught signal SIGPIPE %d\n", signum);
}


int main (int argc, char* argv []) {

	pthread_t thread_check;
	pthread_t thread_broadcast;
	int fdSockClientEmission, fdSockClientReception;
	int fdSockServerEmission = creer_socket_tcp(argv[1]);
    int fdSockServerReception = creer_socket_tcp(argv[2]);

	if(argc != 3){
		perror("Main : Probleme sur le nombre d'argument !\n"); exit(-1);
	}

	signal(SIGPIPE, signal_callback_handler);

	initialiser_de_tous_les_fichiers();


/* Fermeture socket Server (listenning) */
	fermeture_socket_tcp(fdSockServerEmission);
	fermeture_socket_tcp(fdSockServerReception);

	return EXIT_SUCCESS;
}
