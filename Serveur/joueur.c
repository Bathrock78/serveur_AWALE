#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joueur.h"

void init_joueur(Joueur *joueur, const char *pseudo) {
    joueur->score = 0;
    strncpy(joueur->pseudo, pseudo, 15);
    joueur->pseudo[15] = '\0';
}
