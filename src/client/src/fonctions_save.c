#include "fonctions_save.h"


/* Evenement du click sur le bouton d'enregistrement */
/*****************************************************/
void cb_save(GtkWidget *bouton, gpointer pData){

	gchar *ecriture=0;
	GtkWidget *dialog, *p_window;

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->en_cours_de_sauvegarde = 1;

/* Fichier ouvert et utilisateur en cours d'édition */
	if((fichierInfo->file_open == 1) && (fichierInfo->en_cours_dedition != 0)){

		fichierInfo->attente_de_reponse = 1;

		ecriture = recupere_donnees_save(pData);
		printf("Thread Emetteur : ecriture = '%s'\n", ecriture);
		envoyer_save(ecriture, fichierInfo->en_cours_dedition);

	/* window=>vbox=>pToolbar=>le bouton */
		p_window = gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(bouton)));

		dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
		 				 					GTK_BUTTONS_OK, "File saved !");
		gtk_window_set_title(GTK_WINDOW(dialog), "Informations");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		fichierInfo->en_cours_de_sauvegarde = 0;
		fichierInfo->en_cours_dedition = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
		return;
	}

	if((fichierInfo->file_open == 1) && (fichierInfo->en_cours_dedition == 0)){
	/* window=>vbox=>pToolbar=>le bouton */
		p_window = gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(bouton)));

		dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
		 				 					GTK_BUTTONS_OK, "File already saved !");
		gtk_window_set_title(GTK_WINDOW(dialog), "Informations");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		fichierInfo->en_cours_de_sauvegarde = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
		return;
	}

	fichierInfo->en_cours_de_sauvegarde = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
}


/* Cette fonction sert a recuperer la ligne entiere editer par l'utilisateur
le param tmp sert a dissocier le vrai SAVEF CàD quand l'utilisateur appuie sur
le bouton save, des SAVEF envoyer automatiquement tous les 3 carateres ecrit*/
/*****************************************************************************/
char *recupere_donnees_save(gpointer pData){

	int nb_char;
	gchar *ecriture=0;
	GtkTextBuffer *buffer;
	GtkTextIter start_edit, end_edit;

/*on recupere le buffer*/
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pData));

	nb_char = nb_caractere_ligne(&fichierInfo->listeLigne,
										fichierInfo->en_cours_dedition);
	fichierInfo->end_offset += nb_char;

	fichierInfo->end_offset++;

	gtk_text_buffer_get_iter_at_line_offset(buffer, &start_edit,
										fichierInfo->en_cours_dedition-1, 0);

	gtk_text_buffer_get_iter_at_line_offset(buffer, &end_edit,
					fichierInfo->en_cours_dedition-1, fichierInfo->end_offset);

/*recupere dans 'ecriture' le conteu du buffer*/
	ecriture=gtk_text_buffer_get_text(buffer, &start_edit, &end_edit, TRUE);

	fichierInfo->end_offset = -1;
	return ecriture;
}


/* Cette fonction sert a envoyer une requerte SAVEF  de la ligne recuperer au serveur,
	le param tmp sert a dissocier le vrai SAVEF CàD quand l'utilisateur appuie sur le
	bouton save, des SAVEF envoyer automatiquement tous les 3 carateres ecrit  */
/************************************************************************************/
void envoyer_save(char *buffer, int ligne){

	char *envoieMess;
	int taille, taille_buffer;

	taille = strlen(buffer) +  2*sizeof(int) + strlen("SAVEF__") + 1;
	taille_buffer = strlen(buffer);

	envoieMess = initialiser_buffer(taille);

	snprintf(envoieMess, taille, "SAVEF%d_%d_%s", ligne, taille_buffer, buffer);

	printf("Thread Emetteur --> envoieMess : %s\n", envoieMess);

	if(write(fdSockServer, envoieMess, taille) == -1)
 		goto erreur_write_SAVE;

 	fichierInfo->attente_de_reponse = 1;

	return;

	erreur_write_SAVE :
		perror("Thread emetteur : Probleme write SAVEF");
		exit(-1);
}


/* Met à jour le buffer de GTK quand on recoit un BROADCAST 2 cas sont possible :
	-	Utilisateur est en cours d'édition --> on recupere ses données on les met
		dans la liste de ligne et on affiche
	-	l'utilisateur n'est pas en cours d'édition --> alors on affiche la liste
		de lignes
*/
/********************************************************************************/
gboolean cb_update(gpointer p_text_view){

	gchar *ecriture=0;
	GtkTextBuffer *buffer;
	GtkTextIter start_edit, end_edit, iter_place;
	int place_column, place_ligne, nb_char, offset;

    if((fichierInfo->file_open == 1) && (fichierInfo->widget_open == 0)
    	&& (fichierInfo->en_cours_de_sauvegarde == 0)){

    	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p_text_view));
    	gtk_text_buffer_get_iter_at_mark(buffer, &iter_place,
								gtk_text_buffer_get_insert(buffer));

    	place_ligne = gtk_text_iter_get_line(&iter_place);
		place_column = gtk_text_iter_get_line_offset(&iter_place);

	/* Si l'utilisateur n'est pas entrain d'éditer */
	    if((fichierInfo->updated == 1) && (fichierInfo->en_cours_dedition == 0)){
	    	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p_text_view));
			remplir_buffer(buffer);
			nb_char = nb_caractere_ligne(&fichierInfo->listeLigne, (place_ligne + 1));
			if(place_column <= nb_char){
				replacer_curseur(buffer, place_ligne, place_column);
			}
			fichierInfo->updated = 0;
			return TRUE;
	    }

	    if(fichierInfo->en_cours_dedition != 0){
			if((fichierInfo->end_offset >= 0) || (fichierInfo->end_offset < -2)
				|| (fichierInfo->updated == 1)){

			/* Cas de base */
				if(fichierInfo->end_offset == -1){
					offset = 0;
				}
			/* Cas 1er cararctere est un delete ou suppr */
				else if(fichierInfo->end_offset == -2){
					offset = -1;
				}else{
					offset = fichierInfo->end_offset + 1;
				}

				nb_char = nb_caractere_ligne(&fichierInfo->listeLigne,
												fichierInfo->en_cours_dedition);

				offset += nb_char;

				printf("*************** place_column : %d   end_offset : %d\n",
									place_column, offset);

				gtk_text_buffer_get_iter_at_line_offset(buffer, &start_edit, place_ligne, 0);

				gtk_text_buffer_get_iter_at_line_offset(buffer, &end_edit,
											place_ligne, offset);

			/*recupere dans 'ecriture' le conteu du buffer*/
				ecriture=gtk_text_buffer_get_text(buffer, &start_edit, &end_edit, TRUE);

		    	printf("*************** CB_UPDATE *************** ligne : %d  '%s'\n",
		    					fichierInfo->en_cours_dedition, ecriture);

				trouver_et_remplir_ligne(fichierInfo->en_cours_dedition,
												strlen(ecriture), ecriture);

				envoyer_save_temporaire(ecriture, fichierInfo->en_cours_dedition);

		    	remplir_buffer(buffer);
		    	replacer_curseur(buffer, fichierInfo->en_cours_dedition - 1,
		    														place_column);
				fichierInfo->end_offset = -1;

			}
	    }
	}
	fichierInfo->updated = 0;
    return TRUE;
}


/* Cette fonction sert a envoyer une requerte SAVEF  de la ligne recuperer au serveur,
	le param tmp sert a dissocier le vrai SAVEF CàD quand l'utilisateur appuie sur le
	bouton save, des SAVEF envoyer automatiquement tous les 3 carateres ecrit  */
/************************************************************************************/
void envoyer_save_temporaire(char *buffer, int ligne){

	char *envoieMess;
	int taille, taille_buffer;

	taille = strlen(buffer) +  2*sizeof(int) + strlen("SAVET__") + 1;
	taille_buffer = strlen(buffer);

	envoieMess = initialiser_buffer(taille);

	snprintf(envoieMess, taille, "SAVET%d_%d_%s", ligne, taille_buffer, buffer);

	printf("Thread Emetteur --> envoieMess : %s\n", envoieMess);

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	if(write(fdSockServer, envoieMess, taille) == -1)
 		goto erreur_write_SAVET;
 	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

	return;

	erreur_write_SAVET :
		perror("Thread emetteur : Probleme write SAVET");
		exit(-1);
}
