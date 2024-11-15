#ifndef PARTIE_H
#define PARTIE_H

#include "plateau.h"
#include "joueur.h"

struct Partie {
    struct Board *plateau;
    struct Joueur *joueur1;
    struct Joueur *joueur2;
    struct Joueur *joueur_actuel;
    int tour_sans_prise_consecutif;
};

struct Joueur* tirage_au_sort(struct Partie *partie);
bool deplacement(int position, struct Board *board, struct Joueur *joueur);
void capture(int last_position, int *plateau, struct Joueur *joueur);
bool adversaireVide(struct Partie *partie);
bool famine(struct Partie *partie);
bool finDePartie(struct Partie *partie);
struct Joueur* vainqueur(struct Partie *partie);

#endif
