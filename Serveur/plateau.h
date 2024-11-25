#ifndef BOARD_H
#define BOARD_H

typedef struct Plateau {
    int plateau[12];
}Plateau;

void init_board(Plateau *board);
void afficher_plateau(char* buffer, int buf_size, Plateau *board, int score1, const char* pseudo1, int score2, const char* pseudo2,const char* clientPseudo);
int deplacer(int position, int *plateau);
bool test_validite_coup(int *plateau);


#endif
