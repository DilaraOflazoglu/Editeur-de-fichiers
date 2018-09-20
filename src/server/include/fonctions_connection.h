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

int creer_socket_tcp(char *chainePort);

int accepter_connexion(int fdSockServer); /* Par le serveur  */

void fermeture_socket_tcp(int fdSock);

#endif
