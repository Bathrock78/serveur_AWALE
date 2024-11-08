#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int jouerUnCoup(int choix){
    printf("choissisez une case: ");
    scanf("%d",&choix);
    return choix;
}


int main(int argc, char **argv){
    int choix = 0;
    jouerUnCoup(choix);
    return 0;
}