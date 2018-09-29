#Makefile

CC =gcc
CFLAGS =-W -Wall -ansi -pedantic -I./include -std=c11 
LDFLAGS = -lpthread -lrt -I./include

BIN=./bin/
OBJ=./obj/
SRC=./src/
HC=./include/


.SUFFIXES:
.PHONY: all clean test test-server
all: $(BIN)server

test: test-server

test-server: $(BIN)server
	-$$PWD/bin/server 22253 22254 &

$(BIN)%: $(SRC)%.c $(OBJ)fonctions_connection.o $(OBJ)fonctions_client.o $(OBJ)fonctions_fichier.o $(OBJ)fonctions_thread_reception.o $(OBJ)fonctions_thread_broadcast.o $(OBJ)fonctions_thread_emission.o $(OBJ)fonctions_thread_check.o $(OBJ)fonctions_ligne.o
	@if [ -d $(BIN) ]; then : ; else mkdir $(BIN); fi
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

$(OBJ)%.o: $(SRC)%.c $(HC)fonctions_connection.h $(HC)fonctions_client.h $(HC)fonctions_fichier.h $(HC)fonctions_thread_reception.h $(HC)fonctions_thread_broadcast.h $(HC)fonctions_thread_emission.h $(HC)fonctions_thread_check.h $(HC)fonctions_ligne.h
	@if [ -d $(OBJ) ]; then : ; else mkdir $(OBJ); fi
	$(CC) $(CFLAGS) -o $@ -c $<  
		
clean: 
	rm -rf $(OBJ)*.o $(BIN)*
