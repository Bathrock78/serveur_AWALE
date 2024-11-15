#ifndef PARTIE_H
#define PARTIE_H

#include "plateau.h"
#include "joueur.h"

typedef struct {
    struct Board *plateau;
    struct Joueur *joueur1;
    struct Joueur *joueur2;
    struct Joueur *joueur_actuel;
    int tour_sans_prise_consecutif;
}Partie ;

struct Joueur* tirage_au_sort(Partie *partie);
bool deplacement(int position, struct Board *board, struct Joueur *joueur);
void capture(int last_position, int *plateau, struct Joueur *joueur);
bool adversaireVide(Partie *partie);
bool famine(Partie *partie);
bool finDePartie(Partie *partie);
struct Joueur* vainqueur(Partie *partie);

#endif
