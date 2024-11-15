#include <stdio.h>
#include <stdbool.h>
#include "plateau.h"

void init_board(struct Board *board) {
    for (int i = 0; i < 12; i++) {
        board->plateau[i] = 4;
    }
}

void afficher_plateau(struct Board *board) {
    printf("Plateau actuel : \n");

    for (int i = 5; i >= 0; i--) {
        printf("%d (%d)", board->plateau[i], i + 1);
        if (i > 0) {
            printf(" | ");
        }
    }
    printf("\n");

    for (int i = 6; i < 12; i++) {
        printf("%d (%d)", board->plateau[i], i + 1);
        if (i < 11) {
            printf(" | ");
        }
    }
    printf("\n");
}

int deplacer(int position, int *plateau) {
    int jetons_dispos = plateau[position];
    plateau[position] = 0;
    int i = position + 1;
    
    while (jetons_dispos > 0) {
        if (i % 12 != position) {
            plateau[i % 12] += 1;
            jetons_dispos--;
        }
        i++;
    }
    
    return (i - 1) % 12;
}

bool test_validite_coup(int *plateau) {
    int somme_joueur1 = 0;
    int somme_joueur2 = 0;

    for (int i = 0; i < 6; i++) {
        somme_joueur1 += plateau[i];
    }

    for (int i = 6; i < 12; i++) {
        somme_joueur2 += plateau[i];
    }

    return !(somme_joueur1 == 0 || somme_joueur2 == 0);
}