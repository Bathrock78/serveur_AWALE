#ifndef BOARD_H
#define BOARD_H

struct Board {
    int plateau[12];
};

void init_board(struct Board *board);
void afficher_plateau(struct Board *board);
int deplacer(int position, int *plateau);
bool test_validite_coup(int *plateau);

#endif
