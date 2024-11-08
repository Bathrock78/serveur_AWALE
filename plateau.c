#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Board {
    int * plateau[12];
}



void afficher_plateau(Board * board) {
    printf("Plateau actuel : {")
    for (elem : board->plateau){
        printf("%d | ", elem)
    }
    printf("}")
}

int main(int argc, char **argv){
    Board *board = new Board();
    afficher_plateau(&board);
}