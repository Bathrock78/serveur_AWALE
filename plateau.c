#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Board {
    int plateau[12];
};

void afficher_plateau(struct Board *board) {
    printf("Plateau actuel : \n");
    for (int i = 0; i < 12; i++) {
        if (i != 11 && i != 5){
            printf("%d (%d)| ", board->plateau[i], i+1);
        }
        else {
            printf("%d (%d)", board->plateau[i], i+1);
        }
        if (i == 5){
            printf("\n");
        }
    }
    printf("\n");
}

int main(int argc, char **argv) {
    struct Board *board = (struct Board *)malloc(sizeof(struct Board));
    if (board == NULL) {
        printf("Erreur d'allocation mémoire");
    }

    for (int i = 0; i < 12; i++) {
        board->plateau[i] = 0;
    }

    afficher_plateau(board);

    free(board);

    return 0;
}
