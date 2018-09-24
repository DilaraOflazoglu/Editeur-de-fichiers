#include "editeur_gtk.h"


/* Fonction principale de GTK */
/******************************/
void mainGTK(int fdServer, int argc, char* argv []){

    GtkWidget *p_Window, *p_text_view, *vbox, *pToolbar, *scrollbar, *statusbar;
    GtkTextBuffer *buffer;
    gint func_update_ref, func_message_en_vie;

    fdSockServer = fdServer;

    gtk_init(&argc,&argv);  /* Initialisation de GTK+ */

/* Initialisation de toutes les variables */
    p_Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    vbox = gtk_vbox_new(FALSE, 0);
    p_text_view=gtk_text_view_new();
    pToolbar = gtk_toolbar_new();
    scrollbar=gtk_scrolled_window_new(NULL,NULL);
    statusbar = gtk_statusbar_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p_text_view));


/* setteur titre fenetre */
    gtk_window_set_title(GTK_WINDOW(p_Window), "CoopEdit");
/* setteur taille fenetre */
    gtk_window_set_default_size(GTK_WINDOW(p_Window), 800, 800);
/* setteur position fenetre */
    gtk_window_set_position(GTK_WINDOW (p_Window), GTK_WIN_POS_CENTER);

/* Ajout du container vertiale, barre de defilement, bar de menu, zone de texte
dans la fenetre principale */
    gtk_container_add(GTK_CONTAINER(p_Window), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), pToolbar, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(scrollbar), p_text_view);
    gtk_box_pack_start(GTK_BOX(vbox),scrollbar,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

/* insertion des bouton (open, save, edit, fermeture) */
   gtk_toolbar_insert_stock(GTK_TOOLBAR(pToolbar), GTK_STOCK_OPEN, "Open",
      NULL, G_CALLBACK(cb_open), p_text_view, -1);

   gtk_toolbar_insert_stock(GTK_TOOLBAR(pToolbar), GTK_STOCK_SAVE, "Save",
      NULL, G_CALLBACK(cb_save), p_text_view, -1);

   gtk_toolbar_insert_stock(GTK_TOOLBAR(pToolbar), GTK_STOCK_UNDO, "Undo Edit",
      NULL, G_CALLBACK(cb_undo), p_text_view, -1);

   gtk_toolbar_insert_stock(GTK_TOOLBAR(pToolbar), GTK_STOCK_QUIT, "Close",
      NULL, G_CALLBACK(cb_close_bouton), p_text_view, -1);


/* Modification de la taille des icones */
   gtk_toolbar_set_icon_size(GTK_TOOLBAR(pToolbar), GTK_ICON_SIZE_BUTTON);
/* Affichage uniquement des icones */
   gtk_toolbar_set_style(GTK_TOOLBAR(pToolbar), GTK_TOOLBAR_ICONS);

/* Fonction pour l'interraction entre l'utilisateur et la fenetre */
    g_signal_connect(buffer, "changed", G_CALLBACK(update_statusbar), statusbar);
    g_signal_connect_object(buffer, "mark_set", G_CALLBACK(mark_set_callback),
    statusbar, 0);
    g_signal_connect (G_OBJECT (p_Window), "key_press_event",
                                            G_CALLBACK (cb_edit), p_text_view);
    g_signal_connect(G_OBJECT(p_Window), "destroy", G_CALLBACK(cb_close_croix), p_text_view);

    func_update_ref = g_timeout_add(2000, cb_update, p_text_view);

    func_message_en_vie = g_timeout_add(20000, message_en_vie, p_Window);

    update_statusbar(buffer, GTK_STATUSBAR(statusbar));

    gtk_widget_show_all(p_Window);  /*affichage de tous les widgets*/

    gtk_main();     /*demarrage de la boucle evenementielle*/

    g_source_remove (func_update_ref);
    g_source_remove (func_message_en_vie);
}


/* Fonction qui s'éxecute toutes les 20 secondes, elle verifie l'état de
l'application pour voir si tout est en ordre, si le thread crée est en vie */
/***************************************************************************/
gboolean message_en_vie(gpointer p_Window){

/* Cas où le thread Emission n'a pas envoyé de requête au serveur */
    if(fichierInfo->attente_de_reponse == 0){
        if(write(fdSockServer, "ALIVE", strlen("ALIVE")) == -1){
            perror("Thread GTK : Error write ALIVE");   exit(-1);
        }
        fichierInfo->nb_non_reponse = 0;
        return TRUE;
    }

/* Cas où une requête a été envoyé */
    else{

        fichierInfo->nb_non_reponse++;
        printf("******************message_en_vie -->  nb_non_reponse : %d\n",
                                                    fichierInfo->nb_non_reponse);
/* Thread reception ne repond plus */
        if(fichierInfo->nb_non_reponse == 3){
        /* Si l'utilisateur est en cours dedition envoie d'un annul avant */
            if(fichierInfo->en_cours_dedition != 0){
                quitter_sans_enregistrer();
            }
        /* S'il n'est pas en cours d'édition */
            else{
                quitter();
            }
        }
    }
    return TRUE;
}