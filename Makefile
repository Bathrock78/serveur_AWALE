# Nom de l'exécutable
EXEC = awale

# Compilateur et options de compilation
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Les fichiers sources et les fichiers objets correspondants
SRC = main.c plateau.c partie.c joueur.c
OBJ = $(SRC:.c=.o)

# Règle par défaut pour compiler l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Compilation individuelle de chaque fichier source
main.o: main.c plateau.h partie.h joueur.h
	$(CC) $(CFLAGS) -c main.c

plateau.o: plateau.c plateau.h
	$(CC) $(CFLAGS) -c plateau.c

partie.o: partie.c partie.h plateau.h joueur.h
	$(CC) $(CFLAGS) -c partie.c

joueur.o: joueur.c joueur.h
	$(CC) $(CFLAGS) -c joueur.c

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)
