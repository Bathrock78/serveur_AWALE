#ifndef BOARD_H
#define BOARD_H

struct Board {
    int plateau[12];
};

void init_board(struct Board *board);
void afficher_plateau(char* buffer, int buf_size, struct Board *board, int score1, const char* pseudo1, int score2, const char* pseudo2);
int deplacer(int position, int *plateau);
bool test_validite_coup(int *plateau);


#endif
