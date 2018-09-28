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
#include "fonctions_thread_emission.h"
#include "fonctions_thread_reception.h"


/* Catch Signal Handler function */
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
	
	if(pthread_create(&thread_check, NULL, check, NULL) != 0){
		perror("pb invocation pthread_create \n");	exit(EXIT_FAILURE);
	}


/* Attente d'une demande de connexion par le client pour l'accepeter */
	while(1){

		fdSockClientEmission = accepter_connexion(fdSockServerEmission);
        fdSockClientReception = accepter_connexion(fdSockServerReception);

        struct client *new_c = nouveau_client(fdSockClientEmission,
        												fdSockClientReception);

        ajouter_client_liste_temporaire(new_c);

        /* Création du thread qui envoie le fichier */
        if(pthread_create(&new_c->thread_reception, NULL, reception, (void*)new_c)){
            perror("Main : pb invocation pthread_create \n");  exit(EXIT_FAILURE);
        }

        if(pthread_create(&new_c->thread_emission, NULL, emission, (void*)new_c)){
            perror("Main : pb invocation pthread_create \n");  exit(EXIT_FAILURE);
        }
	}
	
	
	if(pthread_join(thread_check, NULL) != 0){
		perror("pb invocation pthread_join \n"); exit(EXIT_FAILURE);
	}


/* Fermeture socket Server (listenning) */
	fermeture_socket_tcp(fdSockServerEmission);
	fermeture_socket_tcp(fdSockServerReception);

	return EXIT_SUCCESS;
}
