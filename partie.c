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


int main(int argc, char **argv){
    int choix = 0;
    jouerUnCoup(choix);
    return 0;
}