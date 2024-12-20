#ifndef PARTIE_H
#define PARTIE_H

#include "plateau.h"
#include "joueur.h"
#include "client2.h"

typedef struct Partie { 
    Plateau *plateau;
    Joueur *joueur1;
    Joueur *joueur2;
    Joueur *joueur_actuel;
    int tour_sans_prise_consecutif;
    Client* client_1;
    Client* client_2;
    time_t debut_tour;
}Partie ;

Joueur* tirage_au_sort(Partie *partie);
bool deplacement(int position, Plateau *board, Joueur *joueur);
void capture(int last_position, int *plateau, Joueur *joueur); 
bool adversaireVide(Partie *partie);
bool famine(Partie *partie);
bool finDePartie(Partie *partie);
Joueur* vainqueur(Partie *partie);
void end_partie(Partie** partie);
void sauvegarder_partie(const char *nomFichier, Partie *partie, int emplacement);
Partie* init_partie(const char* pseudo1, const char * pseudo2);

#endif
