#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void jouerUnCoup(){
    int case;
    scanf("Choisissez une case %d",case);
    printf("case choisie: %d", case);
}


int main(int argc, char **argv){

    jouerUnCoup();
    return 0;
}