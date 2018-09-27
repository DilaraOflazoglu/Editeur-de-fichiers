#include "fonctions_close.h"


/*evenement: fermeture GTK */
/***************************/
void cb_close_croix(GtkWidget *p_window, gpointer *user_data){

	GtkWidget *dialog;

/* Si l'utilisateur est en cours d'édition, on affiche un message
				de voulez vous sauvegarder avant de quiter ? oui non */
	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->widget_open = 1;
	printf("Thread emetteur --> Evenement utilisateur : Close \n");
	if(fichierInfo->en_cours_dedition){
		printf("Thread emetteur --> Utilisateur en cours d'édition \n");
		dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
            "Do you want to save before to quit ?");
  		gtk_window_set_title(GTK_WINDOW(dialog), "Question");
  		gtk_dialog_run(GTK_DIALOG(dialog));

		switch (gtk_dialog_run (GTK_DIALOG (dialog))){

		/* Enregistrer avant de quitter */
			case GTK_RESPONSE_YES:
				gtk_widget_destroy(dialog);
				enregistrer_quitter(p_window, user_data);
				break;

		/* Quitter sans enregistrer */
			case GTK_RESPONSE_NO:
				gtk_widget_destroy(dialog);
				quitter_sans_enregistrer();
				break;

		/* L'utilisateur à appuyer sans faire expres sur fermer */
			case GTK_RESPONSE_CANCEL:
	          	gtk_widget_destroy (dialog);
	          	quitter_sans_enregistrer();
	          	break;

	        break;
		}
		fichierInfo->widget_open = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	}else{
		quitter();
		fichierInfo->widget_open = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	}
}


/*evenement: fermeture GTK */
/***************************/
void cb_close_bouton(GtkWidget *bouton, gpointer *user_data){

	GtkWidget *dialog, *p_window;
	p_window = gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(bouton)));

/* Si l'utilisateur est en cours d'édition, on affiche un message
				de voulez vous sauvegarder avant de quiter ? oui non */
	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->widget_open = 1;
	printf("Thread emetteur --> Evenement utilisateur : Close \n");
	if(fichierInfo->en_cours_dedition){
		printf("Thread emetteur --> Utilisateur en cours d'édition \n");
		dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
            "Do you want to save before to quit ?");
  		gtk_window_set_title(GTK_WINDOW(dialog), "Question");
  		gtk_dialog_run(GTK_DIALOG(dialog));

		switch (gtk_dialog_run (GTK_DIALOG (dialog))){

		/* Enregistrer avant de quitter */
			case GTK_RESPONSE_YES:
				gtk_widget_destroy(dialog);
		  		enregistrer_quitter(p_window, user_data);
				break;

		/* Quitter sans enregistrer */
			case GTK_RESPONSE_NO:
				gtk_widget_destroy(dialog);
				quitter_sans_enregistrer();
				break;

		/* L'utilisateur à appuyer sans faire expres sur fermer */
			case GTK_RESPONSE_CANCEL:
	          	gtk_widget_destroy (dialog);
	          	quitter_sans_enregistrer();
	          	break;

	        break;
		}
		fichierInfo->widget_open = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	}else{
		quitter();
		fichierInfo->widget_open = 0;
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
	}
}


/* Si on est en cours d'édition, on envoie un message au thread recepteur
du serveur pour dire qu'on annule la demande d'edition faite auparavant */
/************************************************************************/
void quitter_sans_enregistrer(){

	if(fichierInfo->en_cours_dedition != 0){

		printf("Thread Emetteur --> envoie : ANNUL%d\n", fichierInfo->en_cours_dedition);

	 	if(write(fdSockServer, "ANNUL", strlen("ANNUL")) == -1){
		    perror("Thread emetteur : Probleme SOCKET SERVEUR A CRASHER");
	 	}


		if(write(fdSockServer, &fichierInfo->en_cours_dedition, sizeof(int)) == -1){
		    perror("Thread emetteur : Probleme SOCKET SERVEUR A CRASHER");
		}
		fichierInfo->en_cours_dedition = 0;
	}

	quitter();
}


/* On envoie un message au thread recepteur
du serveur pour dire qu'on quitte l'application */
/************************************************/
void quitter(){
/* J'AVERTIE LES AUTRES THREADS DE MA MORT */
	printf("Thread Emetteur --> envoie : CLOSE\n");

	if(write(fdSockServer, "CLOSE", strlen("CLOSE")) ==-1){
	    perror("Thread emetteur : Probleme SOCKET SERVEUR A CRASHER");
	 	pthread_cancel(fichierInfo->thread_reception);
	 	fermeture_socket_tcp(fichierInfo->fdSockReception);
	}

	fichierInfo->attente_de_reponse = 1;
	gtk_main_quit();
}


/* Si on est en cours d'édition, on envoie un message au thread recepteur
	du serveur pour dire qu'on fait une sauvegarde de nos modifications */
/************************************************************************/
void enregistrer_quitter(GtkWidget *p_window, gpointer *user_data){

	gchar *ecriture=0;
	GtkWidget *dialog;

	fichierInfo->en_cours_de_sauvegarde = 1;

	fichierInfo->attente_de_reponse = 1;

	ecriture = recupere_donnees_save(user_data);
	printf("Thread Emetteur : ecriture = '%s'\n", ecriture);
	envoyer_save(ecriture, fichierInfo->en_cours_dedition);

	dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
	GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
	 				 					GTK_BUTTONS_OK, "File Saved !");
	gtk_window_set_title(GTK_WINDOW(dialog), "Information");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	fichierInfo->en_cours_dedition = 0;

	quitter();
}
