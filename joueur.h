#ifndef JOUEUR_H
#define JOUEUR_H

struct Joueur {
    int score;
    char pseudo[16];
};

void init_joueur(struct Joueur *joueur, const char *pseudo);

#endif
