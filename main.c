#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plateau.h"
#include "partie.h"
#include "joueur.h"

struct Partie *partie_en_cours;

int main(int argc, char **argv) {
    struct Board *board = (struct Board *)malloc(sizeof(struct Board));
    if (board == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return 1;
    }
    
    init_board(board);
    struct Joueur *joueura = (struct Joueur *)malloc(sizeof(struct Joueur));
    struct Joueur *joueurb = (struct Joueur *)malloc(sizeof(struct Joueur));
    init_joueur(joueura, "Joueur 1");
    init_joueur(joueurb, "Joueur 2");

    partie_en_cours = (struct Partie *)malloc(sizeof(struct Partie));
    partie_en_cours->plateau = board;
    partie_en_cours->joueur1 = joueura;
    partie_en_cours->joueur2 = joueurb;
    partie_en_cours->tour_sans_prise_consecutif = 0;

    partie_en_cours->joueur_actuel = tirage_au_sort(partie_en_cours);

    while (!finDePartie(partie_en_cours)) {
        afficher_plateau(board);
        printf("Tour de %s (score : %d)\n", partie_en_cours->joueur_actuel->pseudo, partie_en_cours->joueur_actuel->score);
        
        int choix;
        printf("Choisissez une case : ");
        scanf("%d", &choix);

        if (choix < 1 || choix > 12) {
            printf("Choix invalide. Veuillez choisir une case entre 1 et 12.\n");
            continue;
        }

        if ((partie_en_cours->joueur_actuel == partie_en_cours->joueur1 && (choix < 1 || choix > 6)) ||
        (partie_en_cours->joueur_actuel == partie_en_cours->joueur2 && (choix < 7 || choix > 12))) {
            printf("Choix invalide. Vous devez choisir une case de votre propre camp.\n");
            continue;
        }

        if (!deplacement(choix - 1, board, partie_en_cours->joueur_actuel)) {
            printf("Déplacement non valide, choisissez une autre case.\n");
            continue;
        }

        partie_en_cours->joueur_actuel = (partie_en_cours->joueur_actuel == partie_en_cours->joueur1) ? partie_en_cours->joueur2 : partie_en_cours->joueur1;
    }

    struct Joueur *gagnant = vainqueur(partie_en_cours);
    if (gagnant != NULL) {
        printf("Le gagnant est %s avec %d points!\n", gagnant->pseudo, gagnant->score);
    }

    free(board);
    free(joueura);
    free(joueurb);
    free(partie_en_cours);

    return 0;
}
