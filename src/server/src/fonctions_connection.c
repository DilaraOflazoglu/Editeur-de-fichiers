#include "fonctions_connection.h"


int creer_socket_tcp(char *chainePort){

/* Variables socket Serveur */
    int fdSockServer;   /* File dscriptor de la socket Serveur */
    struct sockaddr_in maSock;  /* MA socket (Serveur) */


/* Création du socket */
    if((fdSockServer=socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("Probleme socket");
        exit(-1);
    }

/* Initialisation des données de MA socket (Serveur) */
    memset(&maSock, 0, sizeof(struct sockaddr_in));
    maSock.sin_family=AF_INET;
    int port = atoi(chainePort);   /* On change la chaine de caratere en PORT*/
    maSock.sin_port=htons(port);
    maSock.sin_addr.s_addr=htonl(INADDR_ANY);   /* Initialisation de l'adresse IP */


/* Nommage du socket */
    if(bind(fdSockServer, (struct sockaddr *)&maSock, sizeof(maSock))==-1){
        perror("Probleme bind");
        exit(-1);
    }

    if((listen(fdSockServer, 5))==-1){
        perror("Probleme listen");
        exit(-1);
    }
    return fdSockServer;
}


int accepter_connexion(int fdSockServer){

    int fdSockClient;   /* File dscriptor de la socket Client */
    struct sockaddr_in sockExp; /* !!!! Socket EXPEDITEUR (Client) !!!! */
    memset(&sockExp, 0, sizeof(struct sockaddr_in));
    unsigned int tailleSockExp = sizeof(sockExp);   /* Taille de la socket */

/* Attente d'une demande de connexion par le client pour l'accepeter */
/* Appel bloquant */
/* A la connection récupération du file descriptor de la socket Cliente */
    if((fdSockClient=(accept(fdSockServer, (struct sockaddr *)&sockExp, &tailleSockExp)))==-1){
        perror("Probleme accept");
        exit(-1);
    }

    return fdSockClient;
}


void fermeture_socket_tcp(int fdSock){
/* Fermeture socket Client */
    shutdown(fdSock, 2);
    close(fdSock);
}
