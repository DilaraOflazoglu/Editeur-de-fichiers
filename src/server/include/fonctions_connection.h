#ifndef FONCTIONS_CONNECTION_H
#define FONCTIONS_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>


/* Fonction appelé pour créer une socket */
/*****************************************/
int creer_socket_tcp(char *chainePort);


/* Fonction appelé pour accepter une connexion cliente */
/*******************************************************/
int accepter_connexion(int fdSockServer); /* Par le serveur  */


/* Fonction appelé pour fermer une socket */
/*******************************************/
void fermeture_socket_tcp(int fdSock);

#endif
