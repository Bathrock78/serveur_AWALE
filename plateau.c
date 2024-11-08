#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

struct Board {
    int plateau[12];
};

struct Partie {
    struct Board* plateau;
    // joueur 1 a les cases de 1 à 6
    struct Joueur* joueur1;
    // joueur 1 a les cases de 7 à 12
    struct Joueur* joueur2;
    int tour_sans_prise_consecutif;

};

struct Joueur* joueur_actuel;
struct Partie* partie_en_cours;

struct Joueur {
    int score = 0;
    char pseudo[16];

};

void afficher_plateau(struct Board *board) {
    printf("Plateau actuel : \n");

    for (int i = 5; i >= 0; i--) {
        printf("%d (%d)", board->plateau[i], i+1);
        if (i > 0) {
            printf(" | ");
        }
    }
    printf("\n");

    for (int i = 6; i < 12; i++) {
        printf("%d (%d)", board->plateau[i], i+1);
        if (i < 11) {
            printf(" | ");
        }
    }
    printf("\n");
}

int deplacer(int position, int* save_plateau){
    int jetons_dispos = save_plateau[position];
    save_plateau[position] = 0;
    int i = position + 1;
    while (jetons_dispos > 0) {
        if(i%12 != position){
            save_plateau[i % 12] += 1;
            jetons_dispos--;
        }
        i++;
    }
    return i-1;
}

bool test_validite_coup(int* plateau_act){
    int somme = 0;
    for (int i = 0; i<6; i++){
        somme += plateau_act[i];
    }
    if (somme == 0){
        return false;
    }
    somme = 0;
    for (int i = 6; i<12; i++){
        somme += plateau_act[i];
    }
    if (somme == 0){
        return false;
    }
    return true;
}

bool deplacement(int position, struct Board *board){
    if (board->plateau[position]==0){
        printf("Case sélectionnée vide");
        return false;
    }

    int save_plateau[12];
    for (int i = 0; i < 12; i++){
        save_plateau[i] = board->plateau[i];
    }

    int position_end = deplacer(position, save_plateau);

    bool commit = test_validite_coup(save_plateau);
    if (!commit){
        printf("Coup non valide\n");
        return false;
    }
    for (int i = 0; i < 12; i++) {
        board->plateau[i] = save_plateau[i];
    }
    capture(position_end, board->plateau);
    return true;

}

void capture(int last_position, int* plateau){
    bool capture = false;
    while (plateau[last_position] == 2 || plateau[last_position] == 3){
        joueur_actuel->score += plateau[last_position];
        plateau[last_position] = 0;        
        capture = true;
    }
    if (!capture){
        partie_en_cours->tour_sans_prise_consecutif += 1;
    }
}

struct Joueur* tirage_au_sort(){
    int joueur_num = rand()%2;
    if (joueur_num ==0){
        printf("Joueur 1 commence")
        return partie_en_cours->joueur1;
    }
    else{
        printf("Joueur 2 commence")
        return partie_en_cours->joueur2;
    }
}

int jouerUnCoup(int choix){
    printf("choissisez une case: ");
    scanf("%d",&choix);
    return choix;
}

bool adversaireVide(int* board){
    int somme = 0;
    int i = 0;
    //if(Joueur actuel == joueur1){ i = 6} else { i = 0}
    int fin = i + 7;
    for(int i = 0; i < i+7 ;i++){
        somme += board[i];
    }
    
    if(somme == 0){
        printf("Adversaire vide\n");
        return true;
    }else{
        printf("Adversaire non vide\n");
        return false;
    }
}
bool famine(int* board){
    //if(Joueur actuel == joueur1){ i = 5} else { i = 11}
    int i = 5;
    int fin = i + 1;
    while((board[i] + i) < fin){
        i--;
    }
    if( i == fin){
        printf("Famine\n");
        return true;
    }else{
        return false;
    }

}

bool finDePartie(int score, int *board){
    
    if(score >= 25){
        printf("fin par score > 25\n");
        return true;
    }else if(famine(board)){
        printf("fin par famine\n");
        for(int j = 0; j < 12; j++){
            score += board[j];
        }
        return true;
    }else if(/*tour_sans_prise_consecutif == 8*/true){
        print("fin par non prise de graine ");
    }

}

struct Joueur vainqueur(int score, int* board){
    if(finDePartie(score, board)){
        if(/*score_j1 > score_j2*/1){
            printf("Joueur 1 vainqueur");
        }else if (/*score_j2 > score_j1*/1){
            printf("Joueur 2 vainqueur");
        }else{
            printf("Ikiwake");
        }
    }
}

int main(int argc, char **argv) {
    struct Board *board = (struct Board *)malloc(sizeof(struct Board));
    if (board == NULL) {
        printf("Erreur d'allocation mémoire");
    }

    for (int i = 0; i < 12; i++) {
        board->plateau[i] = 9;
    }
    struct Joueur *joueura = (struct Joueur *)malloc(sizeof(struct Joueur));
    struct Joueur *joueurb = (struct Joueur *)malloc(sizeof(struct Joueur));
    partie_en_cours = (struct Partie *)malloc(sizeof(struct Partie));

    partie_en_cours->plateau = board;
    partie_en_cours->joueur1 = joueura;
    partie_en_cours->joueur2 = joueurb;

    joueura->score = 0;
    joueurb->score = 0;

    joueur_actuel = tirage_au_sort();
    afficher_plateau(board);
    bool test = deplacement(5, board);
    afficher_plateau(board);
    printf("%d", test);

    free(board);

    return 0;
}
