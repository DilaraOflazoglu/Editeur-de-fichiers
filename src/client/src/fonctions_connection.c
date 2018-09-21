#include "fonctions_connection.h"


/* L'utilisateur se connecte a la socket du serveur */
/****************************************************/
int connection(char* addrIp, char *chainePort){
    int fdSockServer;  /* File dscriptor de la socket Serveur */
    struct sockaddr_in sockDest;    /* Socket (Serveur) */
    int destLen = sizeof(sockDest); /* Taille de la socket */
	int port;

/* Variable pour la récupération des données du serveur */
    struct addrinfo *result;

/* Création du socket */
    if((fdSockServer=socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("Probleme socket");
        exit(-1);
    }

/* Recuperation info du Serveur */
/* nom du serveur = son adresse IP */
    if(getaddrinfo(addrIp,NULL, NULL, &result) != 0){
        perror("Probleme getaddrinfo");
        exit(-1);
    }

/* Initialisation des données de la socket (Serveur) grâce a getaddrinfo() */
    memset(&sockDest, 0, sizeof(struct sockaddr_in));
    sockDest.sin_family=AF_INET;

    memcpy ((void*)&sockDest.sin_addr,
    (void*)&((struct sockaddr_in*)result->ai_addr)->sin_addr, sizeof(sockDest));

   	port = atoi(chainePort);
    sockDest.sin_port=htons(port);

/* Demande de connexion au Serveur  */
    if((connect(fdSockServer, (struct sockaddr *)&sockDest, destLen))==-1){
        perror("Probleme connect");
        exit(-1);
    }

    return fdSockServer;
}


/* Fermeture de la socket */
/**************************/
void fermeture_socket_tcp(int fdSock){
/* Fermeture socket Client */
     shutdown(fdSock, 2);
     close(fdSock);
}
