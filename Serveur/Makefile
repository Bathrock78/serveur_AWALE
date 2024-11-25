
EXEC = awale

CC = gcc
CFLAGS = -Wall -g

SRC = plateau.c partie.c joueur.c server2.c 
OBJ = $(SRC:.c=.o)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

server2.o: server2.c client2.h server2.h plateau.h partie.h joueur.h 
	$(CC) $(CFLAGS) -c server2.c

plateau.o: plateau.c plateau.h
	$(CC) $(CFLAGS) -c plateau.c

partie.o: partie.c partie.h plateau.h joueur.h
	$(CC) $(CFLAGS) -c partie.c

joueur.o: joueur.c joueur.h
	$(CC) $(CFLAGS) -c joueur.c

clean:
	rm -f $(OBJ) $(EXEC)
