#ifndef JOUEUR_H
#define JOUEUR_H

typedef struct Joueur {
    int score;
    char pseudo[16];
}Joueur;

void init_joueur(Joueur *joueur, const char *pseudo);

#endif
