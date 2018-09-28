#include "fonctions_edit.h"


/* Evenement clavier faite par l'utilisateur */
/*********************************************/
void cb_edit(GtkWidget *p_window, GdkEventKey *event, gpointer data){
	GtkWidget *dialog;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	int ligne, taille, colonne, nb_caracteres;
	char *message;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
	gtk_text_buffer_get_iter_at_mark(buffer, &iter,
												gtk_text_buffer_get_insert(buffer));
	ligne = gtk_text_iter_get_line(&iter) + 1;
	colonne = gtk_text_iter_get_line_offset(&iter);


/* Ajouter toutes les GDK_KEY différentes d'un lettre, d'un chiffre ou d'un caractere */
	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);
	fichierInfo->widget_open = 1;
	if(fichierInfo->file_open == 1){
		if(fichierInfo->en_cours_dedition == 0){

		/* Si l'utilisateur appuie sur entrée au milieu de ligne --> Interdiction */
			if(contains_ligne(&fichierInfo->listeLigneModif, ligne) == 0){
				if(event->keyval == GDK_KEY_Return){
				/* La ligne que l'utilisateur veut editer est en cours d'édition */
					if(contains_ligne(&fichierInfo->listeLigneModif, ligne + 1) == 1){
						taille = 55 + sizeof(int);
						if((message = (char *)malloc(taille*sizeof(int))) == NULL){
							perror("Thread emetteur : Probleme malloc message"); exit(-1);
						}
						snprintf(message, taille,
							"Line %d is being modified by someone else.",
								(ligne + 1));

						event->keyval = NULL;
					  	dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
					            GTK_DIALOG_DESTROY_WITH_PARENT,
					            GTK_MESSAGE_WARNING,
					            GTK_BUTTONS_OK, message);
					  	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
					  	gtk_dialog_run(GTK_DIALOG(dialog));
					  	gtk_widget_destroy(dialog);
					  	free(message);

					  	fichierInfo->widget_open = 0;
						pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
						return;
					}
				}

            /* Suppression de ligne --> plus de caracatere sur le ligne puis touche delete
                ou supprimer pour pour aller a la ligne d'en haut */
				if((event->keyval == GDK_KEY_Return) ||
					(((event->keyval == GDK_KEY_BackSpace)
						|| (event->keyval == GDK_KEY_Delete)) && (colonne == 0))){

					pthread_mutex_lock(&fichierInfo->listeLigne.mutexListeLigne);
					nb_caracteres = nb_caractere_ligne(&fichierInfo->listeLigne, ligne);
					pthread_mutex_unlock(&fichierInfo->listeLigne.mutexListeLigne);

				/* Suppression de ligne */
					if(nb_caracteres == 0){
						taille = 30 + sizeof(int);
						if((message = (char *)malloc(taille*sizeof(int))) == NULL){
							perror("Thread emetteur : Probleme malloc message"); exit(-1);
						}
						snprintf(message, taille,"Do you want to delete line %d ?",
														ligne);

						dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
						GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
				            message);
				  		gtk_window_set_title(GTK_WINDOW(dialog), "Question");
				  		gtk_dialog_run(GTK_DIALOG(dialog));

							switch (gtk_dialog_run(GTK_DIALOG (dialog))){

							/* Suppression ok */
								case GTK_RESPONSE_YES :
									if(write(fdSockServer, "DELET", strlen("DELET")) == -1){
									    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
									}

									if(write(fdSockServer, &ligne, sizeof(int)) == -1){
									    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
									}
									fichierInfo->attente_de_reponse = 1;
								  	break;

					 	 	/* L'utilisateur à appuyer sans faire expres sur le clavier */
								case GTK_RESPONSE_NO:
									//fichierInfo->end_offset++;
									event->keyval = NULL;
									break;

							/* L'utilisateur à appuyer sans faire expres sur le clavier */
								case GTK_RESPONSE_CANCEL:
									//fichierInfo->end_offset++;
									event->keyval = NULL;
					 	  			break;

					 	 		default :
					 	 			//fichierInfo->end_offset++;
					 	 			event->keyval = NULL;
					 	 			break;
							}
							gtk_widget_destroy (dialog);
							fichierInfo->widget_open = 0;
							pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
							free(message);
							return;

					}

					if(colonne < nb_caracteres){

						dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
					        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
					        	GTK_BUTTONS_OK, "You can not edit 2 lines at the same time !");
				 		gtk_window_set_title(GTK_WINDOW(dialog), "Error");
				  		gtk_dialog_run(GTK_DIALOG(dialog));
		  				event->keyval = NULL;

		  				gtk_widget_destroy(dialog);
		  				fichierInfo->widget_open = 0;
						pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
						return;
					}
				}
			}

		/* Si c'est une touche correcte ! */
			if((event->keyval != GDK_KEY_Up) && (event->keyval != GDK_KEY_Down) &&
				(event->keyval != GDK_KEY_Right) && (event->keyval != GDK_KEY_Left) &&
				(event->keyval != GDK_KEY_Shift_R) &&
				(event->keyval != GDK_KEY_Shift_Lock) && (event->keyval != GDK_KEY_Shift_L) &&
				(event->keyval != GDK_KEY_VoidSymbol) && (event->keyval != GDK_KEY_Linefeed) &&
				(event->keyval != GDK_KEY_Escape) && (event->keyval != GDK_KEY_Scroll_Lock) &&
				(event->keyval != GDK_KEY_Sys_Req) && (event->keyval != GDK_KEY_Multi_key) &&
				(event->keyval != GDK_KEY_Codeinput) &&
				(event->keyval != GDK_KEY_ISO_Level3_Shift) &&
				(event->keyval != GDK_KEY_Page_Up) && (event->keyval != GDK_KEY_Page_Down) &&
				(event->keyval != GDK_KEY_End) && (event->keyval != GDK_KEY_Begin) &&
				(event->keyval != GDK_KEY_Prior) && (event->keyval != GDK_KEY_Next) &&
				(event->keyval != GDK_KEY_Select) && (event->keyval != GDK_KEY_Print) &&
				(event->keyval != GDK_KEY_Insert) && (event->keyval != GDK_KEY_Undo) &&
				(event->keyval != GDK_KEY_Redo) && (event->keyval != GDK_KEY_Menu) &&
				(event->keyval != GDK_KEY_Find) && (event->keyval != GDK_KEY_Cancel) &&
				(event->keyval != GDK_KEY_Help) && (event->keyval != GDK_KEY_script_switch) &&
				(event->keyval != GDK_KEY_Mode_switch) && (event->keyval != GDK_KEY_Break) &&
				(event->keyval != GDK_KEY_Num_Lock) && (event->keyval != GDK_KEY_F1) &&
				(event->keyval != GDK_KEY_F2) && (event->keyval != GDK_KEY_F3) &&
				(event->keyval != GDK_KEY_F4) && (event->keyval != GDK_KEY_F5) &&
				(event->keyval != GDK_KEY_F6) && (event->keyval != GDK_KEY_F7) &&
				(event->keyval != GDK_KEY_F8) && (event->keyval != GDK_KEY_F9) &&
				(event->keyval != GDK_KEY_F10) && (event->keyval != GDK_KEY_F11) &&
				(event->keyval != GDK_KEY_F12) && (event->keyval != GDK_KEY_Control_L) &&
				(event->keyval != GDK_KEY_Control_R) && (event->keyval != GDK_KEY_Caps_Lock) &&
				(event->keyval != GDK_KEY_Meta_L) && (event->keyval != GDK_KEY_Meta_R) &&
				(event->keyval != GDK_KEY_Alt_R) && (event->keyval != GDK_KEY_Alt_L)){


					taille = 30 + sizeof(int);
					if((message = (char *)malloc(taille*sizeof(int))) == NULL){
						perror("Thread emetteur : Probleme malloc message"); exit(-1);
					}
					snprintf(message, taille,"Do you want to edit line %d ?",
															ligne);

					dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
					GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
			            message);
			  		gtk_window_set_title(GTK_WINDOW(dialog), "Question");
			  		gtk_dialog_run(GTK_DIALOG(dialog));
			  		free(message);

					switch (gtk_dialog_run(GTK_DIALOG (dialog))){

					/* Début de l'édition */
						case GTK_RESPONSE_YES :
							gtk_widget_destroy(dialog);

							if((event->keyval == GDK_KEY_BackSpace) ||
								(event->keyval == GDK_KEY_Delete)) {
								fichierInfo->end_offset = -2;

							}else if(event->keyval == GDK_KEY_Return){
								fichierInfo->end_offset = -1;
							}else{
								fichierInfo->end_offset = 0;
							}

						/* On n'est pas en cours dédition et personne n'est entrain d'écrire
								sur la ligne qu'on n'essaie d'editer */
							if(contains_ligne(&fichierInfo->listeLigneModif, ligne) == 0){

								if(event->keyval == GDK_KEY_Return){
									fichierInfo->en_cours_dedition = ligne + 1;

									if(write(fdSockServer, "INSRT", strlen("INSRT")) == -1){
									    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
									}

									if(write(fdSockServer, &fichierInfo->en_cours_dedition, sizeof(int)) == -1){
									    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
									}

								}else{
									fichierInfo->en_cours_dedition = ligne;
								}

								sleep(1);

								printf("Thread emetteur --> Fonction Edit Line : %d\n",
														fichierInfo->en_cours_dedition);

							   	if(write(fdSockServer, "EDITF", strlen("EDITF")) == -1){
								    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
								}

								if(write(fdSockServer, &fichierInfo->en_cours_dedition, sizeof(int)) == -1){
								    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
								}

								fichierInfo->attente_de_reponse = 1;
								fichierInfo->en_cours_dedition = 0;

								g_printerr("Thread emetteur KEYVAL : %s 	end_offset : %d\n",
									gdk_keyval_name (event->keyval), fichierInfo->end_offset);

								fichierInfo->widget_open = 0;
								pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
								return;

						/* On n'est pas en cours dédition et une personne est entrain
							d'écrire sur la ligne qu'on n'essaie d'editer */
							}else{

								gtk_widget_destroy (dialog);
								taille = 45 + sizeof(int);
								if((message = (char *)malloc(taille*sizeof(int))) == NULL){
									perror("Thread emetteur : Probleme malloc message"); exit(-1);
								}
								snprintf(message, taille,
									"Line %d is being modified by someone else !", ligne);
								event->keyval = NULL;
							  	dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
							            GTK_DIALOG_DESTROY_WITH_PARENT,
							            GTK_MESSAGE_WARNING,
							            GTK_BUTTONS_OK, message);
							  	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
							  	gtk_dialog_run(GTK_DIALOG(dialog));
							  	gtk_widget_destroy(dialog);
						  	}
						  	fichierInfo->widget_open = 0;
						  	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
						  	free(message);
							return;

			 	 	/* L'utilisateur à appuyer sans faire expres sur le clavier */
						case GTK_RESPONSE_NO:
							event->keyval = NULL;
							break;

					/* L'utilisateur à appuyer sans faire expres sur le clavier */
						case GTK_RESPONSE_CANCEL:
							event->keyval = NULL;
			 	  			break;
			 	 		default :
			 	 			event->keyval = NULL;
			 	 			break;
					}
					gtk_widget_destroy (dialog);
				}
				fichierInfo->widget_open = 0;
				pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
				return;
			}

	/* L'utilisateur essaie d'écrire sur 2 lignes différentes */
		if((fichierInfo->en_cours_dedition != 0) &&
			((ligne != fichierInfo->en_cours_dedition) ||
				(event->keyval == GDK_KEY_Return))){
			if((event->keyval != GDK_KEY_Up) && (event->keyval != GDK_KEY_Left) &&
				(event->keyval != GDK_KEY_Down) && (event->keyval != GDK_KEY_Right)){

				//g_printerr("Thread emetteur KEYVAL : %s\n", gdk_keyval_name (event->keyval));
				taille = 80 + sizeof(int);
				if((message = (char *)malloc(taille*sizeof(int))) == NULL){
					perror("Thread emetteur : Probleme malloc message"); exit(-1);
				}
				snprintf(message, taille,
					"Look out ! You can not edit more than 1 line. \n You are editing line %d.",
						fichierInfo->en_cours_dedition);

				dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
			        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
			        	GTK_BUTTONS_OK, message);
		 		gtk_window_set_title(GTK_WINDOW(dialog), "Error");
		  		gtk_dialog_run(GTK_DIALOG(dialog));
  				event->keyval = NULL;
  				gtk_widget_destroy(dialog);
		  		free(message);
			}
			fichierInfo->widget_open = 0;
			pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
			return;
		}

	/* Incrément du nombre de caracteres taper par l'utlisateur */
		if((fichierInfo->en_cours_dedition != 0) &&
			(ligne == fichierInfo->en_cours_dedition)){

		/* L'utilisateur a tapper une touche qui incrément le compteur d'offset */
			if((event->keyval != GDK_KEY_Return) && (event->keyval != GDK_KEY_Up) &&
				(event->keyval != GDK_KEY_Down) && (event->keyval != GDK_KEY_Right) &&
				(event->keyval != GDK_KEY_Left) && (event->keyval != GDK_KEY_Shift_R) &&
				(event->keyval != GDK_KEY_Shift_Lock) &&
				(event->keyval != GDK_KEY_Shift_L) && (event->keyval != NULL) &&
				(event->keyval != GDK_KEY_BackSpace) &&
				(event->keyval != GDK_KEY_VoidSymbol) && (event->keyval != GDK_KEY_Linefeed) &&
				(event->keyval != GDK_KEY_Escape) && (event->keyval != GDK_KEY_Scroll_Lock) &&
				(event->keyval != GDK_KEY_Sys_Req) && (event->keyval != GDK_KEY_Multi_key) &&
				(event->keyval != GDK_KEY_Codeinput) &&
				(event->keyval != GDK_KEY_ISO_Level3_Shift) &&
				(event->keyval != GDK_KEY_Page_Up) && (event->keyval != GDK_KEY_Page_Down) &&
				(event->keyval != GDK_KEY_End) && (event->keyval != GDK_KEY_Begin) &&
				(event->keyval != GDK_KEY_Prior) && (event->keyval != GDK_KEY_Next) &&
				(event->keyval != GDK_KEY_Select) && (event->keyval != GDK_KEY_Print) &&
				(event->keyval != GDK_KEY_Insert) && (event->keyval != GDK_KEY_Undo) &&
				(event->keyval != GDK_KEY_Redo) && (event->keyval != GDK_KEY_Menu) &&
				(event->keyval != GDK_KEY_Find) && (event->keyval != GDK_KEY_Cancel) &&
				(event->keyval != GDK_KEY_Help) && (event->keyval != GDK_KEY_script_switch) &&
				(event->keyval != GDK_KEY_Mode_switch) && (event->keyval != GDK_KEY_Break) &&
				(event->keyval != GDK_KEY_Num_Lock) && (event->keyval != GDK_KEY_F1) &&
				(event->keyval != GDK_KEY_F2) && (event->keyval != GDK_KEY_F3) &&
				(event->keyval != GDK_KEY_F4) && (event->keyval != GDK_KEY_F5) &&
				(event->keyval != GDK_KEY_F6) && (event->keyval != GDK_KEY_F7) &&
				(event->keyval != GDK_KEY_F8) && (event->keyval != GDK_KEY_F9) &&
				(event->keyval != GDK_KEY_F10) && (event->keyval != GDK_KEY_F11) &&
				(event->keyval != GDK_KEY_F12) && (event->keyval != GDK_KEY_Control_L) &&
				(event->keyval != GDK_KEY_Control_R) && (event->keyval != GDK_KEY_Caps_Lock) &&
				(event->keyval != GDK_KEY_Meta_L) && (event->keyval != GDK_KEY_Meta_R) &&
				(event->keyval != GDK_KEY_Alt_R) && (event->keyval != GDK_KEY_Delete) &&
				(event->keyval != GDK_KEY_Alt_L)){

				fichierInfo->end_offset++;
				fichierInfo->widget_open = 0;
				pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
				return;
			}

		/* Si c'est la touche delete ou suppr */
			if((event->keyval == GDK_KEY_BackSpace) ||
				(event->keyval == GDK_KEY_Delete)){

				fichierInfo->end_offset--;
				g_printerr("Thread emetteur KEYVAL : %s  offset : %d\n",
				            gdk_keyval_name (event->keyval), fichierInfo->end_offset);

				pthread_mutex_lock(&fichierInfo->listeLigne.mutexListeLigne);
				nb_caracteres = nb_caractere_ligne(&fichierInfo->listeLigne, ligne);
				pthread_mutex_unlock(&fichierInfo->listeLigne.mutexListeLigne);

		/* L'utilisateur a supprimer sa ligne d'édition il perd le droit d'éditer */
				if((nb_caracteres + fichierInfo->end_offset) == -2){

					taille = 30 + sizeof(int);
					if((message = (char *)malloc(taille*sizeof(int))) == NULL){
						perror("Thread emetteur : Probleme malloc message"); exit(-1);
					}
					snprintf(message, taille,"Do you want to delete line  %d ?",
													fichierInfo->en_cours_dedition);

					dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
					GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
			            message);
			  		gtk_window_set_title(GTK_WINDOW(dialog), "Question");
			  		gtk_dialog_run(GTK_DIALOG(dialog));

					switch (gtk_dialog_run(GTK_DIALOG (dialog))){

					/* Suppression ok */
						case GTK_RESPONSE_YES :
							if(write(fdSockServer, "DELET", strlen("DELET")) == -1){
							    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
							}

							if(write(fdSockServer, &fichierInfo->en_cours_dedition, sizeof(int)) == -1){
							    perror("Thread emetteur : Probleme write OPENF");  exit(-1);
							}

							fichierInfo->attente_de_reponse = 1;
						  	break;

			 	 	/* L'utilisateur à appuyer sans faire expres sur le clavier */
						case GTK_RESPONSE_NO:
							printf("ICIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n");
							//fichierInfo->end_offset++;
							g_printerr("Thread emetteur KEYVAL : %s   offset : %d\n",
							         gdk_keyval_name (event->keyval), fichierInfo->end_offset);
							event->keyval = NULL;
							break;

					/* L'utilisateur à appuyer sans faire expres sur le clavier */
						case GTK_RESPONSE_CANCEL:
							//fichierInfo->end_offset++;
							event->keyval = NULL;
			 	  			break;

			 	 		default :
			 	 			//fichierInfo->end_offset++;
			 	 			event->keyval = NULL;
			 	 			break;
					}
					gtk_widget_destroy (dialog);
					fichierInfo->widget_open = 0;
					pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
					free(message);
					return;
				}

		/* Cas ou on supprime jusqu'a (en_cours_dedition-1) alors qu'on est au milieu d'un ligne */
				/* Suppression retour a la ligne Interdit */
				if(((nb_caracteres + fichierInfo->end_offset) > -2) &&
							(colonne == 0)){

					taille = 51 + sizeof(int);
					if((message = (char *)malloc(taille*sizeof(int))) == NULL){
						perror("Thread emetteur : Probleme malloc message"); exit(-1);
					}
					snprintf(message, taille,
					"You have not permissions to edit line %d !",
						fichierInfo->en_cours_dedition);

					event->keyval = NULL;
					dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
				        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
				        	GTK_BUTTONS_OK, message);
			 		gtk_window_set_title(GTK_WINDOW(dialog), "Error");
			  		gtk_dialog_run(GTK_DIALOG(dialog));

			  		gtk_widget_destroy (dialog);
					fichierInfo->widget_open = 0;
					pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
					free(message);
					return;
				}
				fichierInfo->widget_open = 0;
				pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
				return;
			}
		}
	}else{
		event->keyval = NULL;
		dialog = gtk_message_dialog_new(GTK_WINDOW(p_window),
	        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
	        	GTK_BUTTONS_OK, "You can not edit without opening file !");
 		gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  		gtk_dialog_run(GTK_DIALOG(dialog));
  		gtk_widget_destroy(dialog);
	}

	fichierInfo->widget_open = 0;
	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
}


/* Evenement curseur : pour avoir le numero de ligne est le numero de colonne */
/******************************************************************************/
void update_statusbar(GtkTextBuffer *buffer, GtkStatusbar  *statusbar){

	gchar *msg;
	gint row, col;
	GtkTextIter iter;

	gtk_statusbar_pop(statusbar, 0);

	gtk_text_buffer_get_iter_at_mark(buffer, &iter,
								gtk_text_buffer_get_insert(buffer));

	row = gtk_text_iter_get_line(&iter);
	col = gtk_text_iter_get_line_offset(&iter);

/* Si ce n'est pas la ligne que j'édite */
	pthread_mutex_lock(&fichierInfo->mutexVariablesGTK);

	if(fichierInfo->en_cours_dedition ==  (row + 1)){
		msg = g_strdup_printf("You are editing this line.					Line: %d  Column: %d", row+1, col+1);

/* Si la ligne que j'essaie d'éditer est en cours d'édition par un autre utilisateur */
	}else if((contains_ligne(&fichierInfo->listeLigneModif, row+1) == 1)
			&& (fichierInfo->en_cours_dedition != (row + 1))){
		msg = g_strdup_printf("Line is being edited...								Line: %d  Column: %d", row+1, col+1);
	}else if((fichierInfo->en_cours_dedition != 0) &&
					(fichierInfo->en_cours_dedition !=  (row + 1))){
		msg = g_strdup_printf("Your cursor is not on the right line !	Line: %d  Column: %d",
							row+1, col+1);

	}else if(contains_ligne(&fichierInfo->listeLigneModif, row+1) == 0){
		msg = g_strdup_printf("Line: %d						Column: %d", row+1, col+1);

	}else{
		pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);
		return;
	}

	pthread_mutex_unlock(&fichierInfo->mutexVariablesGTK);

	gtk_statusbar_push(statusbar, 0, msg);

	g_free(msg);
}


/* Evenement curseur */
/*********************/
void mark_set_callback(GtkTextBuffer *buffer,
    const GtkTextIter *new_location, GtkTextMark *mark, gpointer data){

  	update_statusbar(buffer, GTK_STATUSBAR(data));
}
