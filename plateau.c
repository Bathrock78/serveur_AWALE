#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

struct Board {
    int plateau[12];
};

struct Partie {
    struct Board plateau;
    // joueur 1 a les cases de 1 à 6
    struct Joueur joueur1;
    // joueur 1 a les cases de 7 à 12
    struct Joueur joueur2;

};

struct Joueur {
    int score;
};

void afficher_plateau(struct Board *board) {
    printf("Plateau actuel : \n");

    for (int i = 5; i >= 0; i--) {
        printf("%d (%d)", board->plateau[i], i+1);
        if (i > 0) {
            printf(" | ");
        }
    }
    printf("\n");

    for (int i = 6; i < 12; i++) {
        printf("%d (%d)", board->plateau[i], i+1);
        if (i < 11) {
            printf(" | ");
        }
    }
    printf("\n");
}

void deplacer(int position, int* save_plateau){
    int jetons_dispos = save_plateau[position];
    save_plateau[position] = 0;
    int i = position + 1;
    while (jetons_dispos > 0) {
        save_plateau[i % 12] += 1;
        jetons_dispos--;
        i++;
    }
}

bool test_validite_coup(int* plateau_act){
    int somme = 0;
    for (int i = 0; i<6; i++){
        somme += plateau_act[i];
    }
    if (somme == 0){
        return false;
    }
    somme = 0;
    for (int i = 6; i<12; i++){
        somme += plateau_act[i];
    }
    if (somme == 0){
        return false;
    }
    return true;
}

bool deplacement(int position, struct Board *board){
    if (board->plateau[position]==0){
        printf("position sélectionnée vide");
        return false;
    }
    int save_plateau[12];
    for (int i = 0; i < 12; i++){
        save_plateau[i] = board->plateau[i];
    }

    deplacer(position, save_plateau);

    bool commit = test_validite_coup(save_plateau);
    if (!commit){
        printf("Coup non valide\n");
        return false;
    }
    for (int i = 0; i < 12; i++) {
        board->plateau[i] = save_plateau[i];
    }
    //capture
    return true;

}




int main(int argc, char **argv) {
    struct Board *board = (struct Board *)malloc(sizeof(struct Board));
    if (board == NULL) {
        printf("Erreur d'allocation mémoire");
    }

    for (int i = 0; i < 12; i++) {
        board->plateau[i] = 9;
    }

    afficher_plateau(board);
    bool test = deplacement(5, board);
    afficher_plateau(board);
    printf("%d", test);

    free(board);

    return 0;
}
