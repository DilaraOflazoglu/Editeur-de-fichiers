#ifndef FONCTIONS_THREAD_CHECK_H
#define FONCTIONS_THREAD_CHECK_H

#define _POSIX_C_SOURCE 200809L
#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include "fonctions_client.h"
#include "structures.h"

void *check();
void check_one_file(struct fichier *file);
void check_liste_temporaire();

void suppression_client_en_panne(struct fichier *file, struct client *clt);

#endif
