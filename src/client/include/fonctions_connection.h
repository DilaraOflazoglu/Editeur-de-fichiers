#ifndef FONCTIONS_CONNECTION_H
#define FONCTIONS_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/un.h>
#include <dirent.h>
#include <sys/socket.h>


/* Tentative de conection par le client */
int connection(char* addrIp, char *chainePort);	

void fermeture_socket_tcp(int fdSock);

#endif
