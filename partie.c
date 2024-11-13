#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plateau.h"
#include "partie.h"
#include "joueur.h"

struct Joueur* tirage_au_sort(struct Partie *partie) {
    int joueur_num = rand() % 2;
    if (joueur_num == 0) {
        printf("Joueur 1 commence\n");
        return partie->joueur1;
    } else {
        printf("Joueur 2 commence\n");
        return partie->joueur2;
    }
}

bool deplacement(int position, struct Board *board, struct Joueur *joueur) {
    if (board->plateau[position] == 0) {
        printf("Case sélectionnée vide\n");
        return false;
    }

    int save_plateau[12];
    for (int i = 0; i < 12; i++) {
        save_plateau[i] = board->plateau[i];
    }

    int position_end = deplacer(position, save_plateau);
    bool commit = test_validite_coup(save_plateau);

    if (!commit) {
        printf("Coup non valide\n");
        return false;
    }

    for (int i = 0; i < 12; i++) {
        board->plateau[i] = save_plateau[i];
    }

    capture(position_end, board->plateau, joueur);
    return true;
}

void capture(int last_position, int* plateau, struct Joueur *joueur) {
    while (plateau[last_position] == 2 || plateau[last_position] == 3) {
        joueur->score += plateau[last_position];
        plateau[last_position] = 0;
        last_position--;
    }
}


bool adversaireVide(struct Partie *partie) {
    int somme = 0;
    int debut = (partie->joueur1 == partie->joueur_actuel) ? 6 : 0;
    int fin = debut + 6;

    for (int i = debut; i < fin; i++) {
        somme += partie->plateau->plateau[i];
    }
    
    if (somme == 0) {
        printf("Adversaire vide\n");
        return true;
    } else {
        printf("Adversaire non vide\n");
        return false;
    }
}
bool famine(struct Partie *partie){
    int i = (partie->joueur1 == partie->joueur_actuel) ? 5 : 11;
    int fin = i + 1;
    while((partie->plateau->plateau[i] + i) < fin){
        i--;
    }
    if( i == fin){
        printf("Famine\n");
        return true;
    }else{
        return false;
    }

}

bool finDePartie(struct Partie *partie) {

    if (partie->joueur1->score >= 25 || partie->joueur2->score >= 25) {
        printf("Fin de partie : score supérieur à 25\n");
        return true;
    }
    if (famine(partie)) {
        printf("Fin de partie par famine\n");
        for (int i = 0; i < 12; i++) {
            if (i < 6) {
                partie->joueur1->score += partie->plateau->plateau[i];
            } else {
                partie->joueur2->score += partie->plateau->plateau[i];
            }
        }
        return true;
    }
    if (partie->tour_sans_prise_consecutif >= 8) {
        printf("Fin de partie par absence de prise prolongée\n");
        return true;
    }

    return false;
}


struct Joueur* vainqueur(struct Partie *partie) {
    if (finDePartie(partie)) {
        if (partie->joueur1->score > partie->joueur2->score) {
            printf("Joueur 1 vainqueur\n");
            return partie->joueur1;
        } else if (partie->joueur2->score > partie->joueur1->score) {
            printf("Joueur 2 vainqueur\n");
            return partie->joueur2;
        } else {
            printf("Ikiwake\n");
            return NULL;
        }
    }
    return NULL;
}