#Makefile

CC =gcc
CFLAGS =-W -Wall -ansi -pedantic -I./include -std=c11 `pkg-config --cflags --libs gtk+-2.0` # les options pour le compilateur
LDFLAGS = -lpthread -lrt -I./include
#`pkg-config --cflags --libs gtk+-3.0` # les options pour l'éditeur de liens

BIN=./bin/
OBJ=./obj/
SRC=./src/
HC=./include/


.SUFFIXES:
.PHONY: all clean test-client
all: $(BIN)client

test: test-client

test-client: $(BIN)client
	-$$PWD/bin/client 127.0.0.1 22253 22254


$(BIN)%: $(SRC)%.c $(OBJ)fonctions_connection.o $(OBJ)fonctions_fichier.o $(OBJ)fonctions_undo.o $(OBJ)fonctions_open.o $(OBJ)fonctions_save.o $(OBJ)fonctions_ligne.o $(OBJ)editeur_gtk.o $(OBJ)fonctions_reception.o $(OBJ)fonctions_edit.o $(OBJ)fonctions_close.o 
	@if [ -d $(BIN) ]; then : ; else mkdir $(BIN); fi
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

$(OBJ)%.o: $(SRC)%.c $(HC)fonctions_connection.h $(HC)fonctions_undo.h $(HC)fonctions_fichier.h  $(HC)fonctions_open.h $(HC)fonctions_save.h $(HC)editeur_gtk.h $(HC)fonctions_ligne.h $(HC)fonctions_reception.h $(HC)fonctions_edit.h $(HC)fonctions_close.h
	@if [ -d $(OBJ) ]; then : ; else mkdir $(OBJ); fi
	$(CC) $(CFLAGS) -o $@ -c $<  
		
clean: 
	rm -rf $(OBJ)*.o $(BIN)*
