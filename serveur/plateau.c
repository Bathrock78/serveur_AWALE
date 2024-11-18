#include <stdio.h>
#include <stdbool.h>
#include "plateau.h"

void init_board(struct Board *board) {
    for (int i = 0; i < 12; i++) {
        board->plateau[i] = 4;
    }
}

void afficher_plateau(char* buffer, int BUF_SIZE, struct Board *board, int score1, const char* pseudo1, int score2, const char* pseudo2) {
    strncpy(buffer, "\n", BUF_SIZE - 1);
    char mini_buffer[BUF_SIZE];
    snprintf(mini_buffer, BUF_SIZE, "Score : %s - %d | %s - %d\n", pseudo1, score1, pseudo2, score2);
    strncat(buffer, mini_buffer, BUF_SIZE - strlen(buffer) - 1);
    strncat(buffer, "Plateau actuel : \n", BUF_SIZE - strlen(buffer) - 1);

    for (int i = 5; i >= 0; i--) {
        char mini_buffer[BUF_SIZE];
        snprintf(mini_buffer, BUF_SIZE, "%d (%d)", board->plateau[i], i + 1);
        strncat(buffer, mini_buffer, BUF_SIZE - strlen(buffer) - 1);
        if (i > 0) {
            strncat(buffer, " | ", BUF_SIZE - strlen(buffer) - 1);
        }
    }
    strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);

    for (int i = 6; i < 12; i++) {
        char mini_buffer[BUF_SIZE];
        snprintf(mini_buffer, BUF_SIZE, "%d (%d)", board->plateau[i], i + 1);
        strncat(buffer, mini_buffer, BUF_SIZE - strlen(buffer) - 1);
        if (i < 11) {
            strncat(buffer, " | ", BUF_SIZE - strlen(buffer) - 1);
        }
    }
    strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
    return buffer;
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