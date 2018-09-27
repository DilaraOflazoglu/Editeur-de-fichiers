#include "fonctions_open.h"


static GtkWidget *file_selector=NULL;


/*evenement: ouverture fichier*/
/******************************/
void cb_open(GtkWidget *p_widget, gpointer user_data){

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	if(fichierInfo->file_open != 1){
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

		GtkWidget *file = GTK_WIDGET(user_data);

		file_selector = gtk_file_selection_new("Select file");
	 	gtk_widget_show(file_selector);

		g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(file_selector)->ok_button),
							"clicked", G_CALLBACK(ouvrir_fichier), file);

		g_signal_connect_swapped(G_OBJECT(GTK_FILE_SELECTION(file_selector)->cancel_button),
		 "clicked", G_CALLBACK(gtk_widget_destroy), file_selector);
	}else{
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	}
}


/*procedure d'ouverture de fichier*/
/**********************************/
void ouvrir_fichier(GtkWidget *bouton, GtkWidget *text_view){

	char *chemin;
	GtkTextBuffer *buffer;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
/*recupere le chemin du fichier qu'on a selectioné*/
	chemin = gtk_file_selection_get_filename(GTK_FILE_SELECTION (file_selector));

	g_signal_connect_swapped(G_OBJECT(GTK_FILE_SELECTION(file_selector)->cancel_button),
	 "clicked", G_CALLBACK(gtk_widget_destroy), file_selector);

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->attente_de_reponse = 1;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	gtk_widget_destroy(file_selector);  /*supprime la boite de selection*/

/* Remplir le nom du fichier + attendre que le fichier soit rempi */
	remplir_fileName(chemin);

	remplir_buffer(buffer);

	free(chemin);
	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->file_open = 1;
	fichierInfo->en_cours_dedition = 0;
	fichierInfo->end_offset = 0;
	fichierInfo->en_cours_de_sauvegarde = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	remove_directory();
}


/* Affichage des lignes de la structure liste ligne dans le buffer de GTK */
/**************************************************************************/
void remplir_buffer(GtkTextBuffer *buffer){

	char *buffer_text;
	struct ligneC *cible;
	GtkTextIter start, end;

	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_delete(buffer, &start, &end);  /*supprime le contenu du buffer*/

	pthread_mutex_lock(&fichierInfo->listeLigne.mutexListeLigne);
	cible = fichierInfo->listeLigne.tabLigne.next;

	while(cible != &fichierInfo->listeLigne.tabLigne){
		printf("remplir_buffer --> affichage Ligne : %d Taille : %d Buffer : '%s'\n",
						cible->num, cible->taille, cible->buffer);

		gtk_text_buffer_get_end_iter(buffer, &end);
		if(cible->taille != 0){
			buffer_text = initialiser_buffer(cible->taille + 1);
			snprintf(buffer_text, (cible->taille+1), "%s", cible->buffer);

			gtk_text_buffer_insert(buffer, &end,
				g_locale_to_utf8(buffer_text, -1, NULL,	NULL, NULL), -1);
			free(buffer_text);
		}

		if(cible->next != &fichierInfo->listeLigne.tabLigne){
			gtk_text_buffer_insert(buffer, &end,
				g_locale_to_utf8("\n", -1, NULL, NULL, NULL), -1);
		}
		cible = cible->next;
	}
	pthread_mutex_unlock(&fichierInfo->listeLigne.mutexListeLigne);
}


/* Lors d'une MàJ du buffer on remet le curseur a la place avant la MàJ */
/************************************************************************/
void replacer_curseur(GtkTextBuffer *buffer, int place_ligne, int place_column){

	GtkTextIter replace_iter;

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	if(fichierInfo->widget_open == 0){

		printf("-------------REPLACE CURSOR line : %d column : %d udated : %d ------------\n",
						place_ligne, place_column, fichierInfo->updated);

		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

		printf("-------------REPLACE CURSOR %d %d------------\n",
											place_ligne, place_column);

		gtk_text_buffer_get_iter_at_line_offset(buffer, &replace_iter,
						place_ligne, place_column);

		gtk_text_buffer_place_cursor (buffer, &replace_iter);
		return;
	}

	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

}


/* Extraction du nom du fichier a partir chemin et
initialisation du nom du fichier  de la structure commande */
/***********************************************************/
void remplir_fileName(char *chemin){

   const char caractere[2] = "/";
   char *token_last, *token_previous, *p, *fileName;
   int taille;

/* On recupere le premier et le 2 repertoire */
   	p = strtok(chemin, caractere);

	token_last = strdup(p);

	p = strtok(NULL, caractere);
/* Et on parcourt les suivants un a un */
   	while(p != NULL){

      	token_previous = strdup(token_last);
      	token_last = strdup(p);

      	p = strtok(NULL, caractere);
   	}

   	taille = strlen(token_previous) + strlen(token_last) + 2;

   	fileName = initialiser_buffer(taille);
   	snprintf(fileName, taille, "%s/%s", token_previous, token_last);

   	fichierInfo->fileName = strdup(fileName);

    printf("fileName : %s\n", fichierInfo->fileName);

    taille = strlen(token_last);

    pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);

   	if(write(fdSockServer, "OPENF", strlen("OPENF")) == -1){
	    perror("Thread emetteur : Probleme write OPENF");
	    goto error_out_1;
	}

    if(write(fdSockServer, &taille, sizeof(int)) ==-1){
	    perror("Thread emetteur : Probleme write taille");
	    goto error_out_1;
	}

    if(write(fdSockServer, token_last, strlen(token_last)) ==-1){
	    perror("Thread emetteur : Probleme write fileName");
	    goto error_out_1;
	}

   	free(fileName);
   	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

/* nom du fichier rempli, signaler le thread de reception */
	pthread_mutex_lock(&fichierInfo->mutexfileName);
	fichierInfo->fileName_rempli = 1;
	pthread_cond_signal(&fichierInfo->condfileName);

/* Attendre que le fichier soit rempli */
	if(fichierInfo->fileName_rempli == 1){
		pthread_cond_wait(&fichierInfo->condfileName, &fichierInfo->mutexfileName);
	}
	pthread_mutex_unlock(&fichierInfo->mutexfileName);

	return;


error_out_1 :
	free(fileName);
	exit(-1);
}
