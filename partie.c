#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

int jouerUnCoup(int choix){
    printf("choissisez une case: ");
    scanf("%d",&choix);
    return choix;
}

int JoueurVide(int* board){
    int somme_1 = 0;
    int somme_2 = 0;
    for(int i = 0; i < 7;i++){
        somme_1 += board[i];
    }
    for(int i = 6; i < 13;i++){
        somme_2 += board[i];
    }
    if(somme_1 == 0){
        printf("Joueur 1 vide\n");
        return 1;
    }else if(somme_2 == 0){
        printf("Joueur 1 vide\n");
        return 2;
    }else{
        printf("Aucun joueur vide");
        return 0;
    }
}
int famine(int* board){
    int joueur_vide = JoueurVide(board);
    switch (joueur_vide)
    {
    case 1:
        int i = 11;
        while(board[i] + i < 12){
            i--;
        }
        if( i == 6){
            printf("Famine: Victoire du joueur 2");
            return 2;
        }
        /*for(int j = 0; j < 13; j++){
            score += board[j];
        }*/
        /* code */
        break;
    case 2:
        int i = 5;
        while(board[i] + i < 6){
            i--;
        }
        if( i == 0){
            printf("Famine: Victoire du joueur 1");
            return 1;
        }
        /*code*/
        break;
    default:
        break;
    }

}

bool partieGagnee(int score, int *board){

    if(score >= 25){
        return true;
    }else if(famine(board)){
        
    }

}


int main(int argc, char **argv){
    int choix = 0;
    jouerUnCoup(choix);
    return 0;
}