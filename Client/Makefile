EXEC = client

CC = gcc
CFLAGS = -Wall

SRC = client2.c 
OBJ = $(SRC:.c=.o)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

client2.o: client2.c client2.h 
	$(CC) $(CFLAGS) -c server2.c

clean:
	rm -f $(OBJ) $(EXEC)
