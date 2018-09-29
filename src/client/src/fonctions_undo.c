#include "fonctions_undo.h"


/* Evenement du click sur le bouton d'annulation d'édition */
/***********************************************************/
void cb_undo(GtkWidget *bouton, gpointer pData){

	GtkWidget *dialog, *p_window;
	struct ligneC *cible;
	GtkTextBuffer *buffer;

	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->widget_open = 1;
	/* Fichier ouvert et utilisateur en cours d'édition */
	/* L'utilisateur veut annuler sa demande d'édition */
	if((fichierInfo->file_open == 1) && (fichierInfo->en_cours_dedition != 0)){

		printf("Thread Emetteur --> envoie : ANNUL%d\n",
											fichierInfo->en_cours_dedition);

	 	if(write(fdSockServer, "ANNUL", strlen("ANNUL")) == -1){
		    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
		}

		if(write(fdSockServer, &fichierInfo->en_cours_dedition, sizeof(int)) == -1){
		    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
		}

		pthread_mutex_lock(&fichierInfo->listeLigneModif.mutexListeLigne);
		cible = fichierInfo->listeLigneModif.tabLigne.next;

		while(cible != &fichierInfo->listeLigneModif.tabLigne){
	        if(cible->num == fichierInfo->en_cours_dedition){
		/* Suppression, remplissage des données et MAJ du chainage */
	        	supprimer_ligne(cible);
	        	detruire(cible);
	         	break;
		    }
		    cible = cible->next;
		}
		pthread_mutex_unlock(&fichierInfo->listeLigneModif.mutexListeLigne);

		fichierInfo->en_cours_dedition = 0;
		fichierInfo->widget_open = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

	/*on recupere le buffer*/
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pData));
		remplir_buffer(buffer);
		return;
	}

/* Pas en cours d'édition pour annuler */
	if((fichierInfo->file_open == 1) && (fichierInfo->en_cours_dedition == 0)){

		p_window = gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(bouton)));

		dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
		 				 					GTK_BUTTONS_OK, "You are not editing. You can not undo !");
		gtk_window_set_title(GTK_WINDOW(dialog), "Error");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		fichierInfo->widget_open = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
		return;
	}

	fichierInfo->widget_open = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
}
