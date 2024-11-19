#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plateau.h"
#include "partie.h"
#include "joueur.h"
#include <unistd.h>

Joueur* tirage_au_sort(Partie *partie) {
    int joueur_num = rand() % 2;
    if (joueur_num == 0) {
        printf("Joueur 1 commence\n");
        return partie->joueur1;
    } else {
        printf("Joueur 2 commence\n");
        return partie->joueur2;
    }
}

bool deplacement(int position, Plateau *board, Joueur *joueur) {
    if (board->plateau[position] == 0) {
        printf("Case sélectionnée vide\n");
        return false;
    }

    int save_plateau[12];
    for (int i = 0; i < 12; i++) {
        save_plateau[i] = board->plateau[i];
    }

    int position_end = deplacer(position, save_plateau);
    bool commit = test_validite_coup(save_plateau);

    if (!commit) {
        printf("Coup non valide\n");
        return false;
    }

    for (int i = 0; i < 12; i++) {
        board->plateau[i] = save_plateau[i];
    }

    capture(position_end, board->plateau, joueur);
    return true;
}

void capture(int last_position, int* plateau, Joueur *joueur) {
    while (plateau[last_position] == 2 || plateau[last_position] == 3) {
        joueur->score += plateau[last_position];
        plateau[last_position] = 0;
        last_position--;
    }
}


bool adversaireVide(Partie *partie) {
    int somme = 0;
    int debut = (partie->joueur1 == partie->joueur_actuel) ? 6 : 0;
    int fin = debut + 6;

    for (int i = debut; i < fin; i++) {
        somme += partie->plateau->plateau[i];
    }
    
    if (somme == 0) {
        printf("Adversaire vide\n");
        return true;
    } else {
        printf("Adversaire non vide\n");
        return false;
    }
}
bool famine(Partie *partie){
    /*int i = (partie->joueur1 == partie->joueur_actuel) ? 5 : 11;
    int fin = i + 1;
    while((partie->plateau->plateau[i] + i) < fin){
        i--;
    }
    if( i == fin){
        printf("Famine\n");
        return true;
    }else{
        return false;
    }*/
    int debut = (partie->joueur1 == partie->joueur_actuel) ? 0 : 6;
    int fin = debut + 6;

    for (int i = debut; i < fin; i++) {
        int graines = partie->plateau->plateau[i];
        if (graines > 0) {
            int positionFinale = (i + graines) % 12; // Position finale après le semis
            if ((partie->joueur1 == partie->joueur_actuel && positionFinale >= 6) ||
                (partie->joueur1 != partie->joueur_actuel && positionFinale < 6)) {
                // Si la dernière graine tombe dans le camp adverse
                return false;
            }
        }
    }
    printf("Famine\n");
    return true;
}

bool finDePartie(Partie *partie) {

    if (partie->joueur1->score >= 25 || partie->joueur2->score >= 25) {
        printf("Fin de partie : score supérieur à 25\n");
        return true;
    }
    if (famine(partie)) {
        printf("Fin de partie par famine\n");
        for (int i = 0; i < 12; i++) {
            if (i < 6) {
                partie->joueur1->score += partie->plateau->plateau[i];
            } else {
                partie->joueur2->score += partie->plateau->plateau[i];
            }
        }
        return true;
    }
    if (partie->tour_sans_prise_consecutif >= 8) {
        printf("Fin de partie par absence de prise prolongée\n");
        return true;
    }

    return false;
}


Joueur* vainqueur(Partie *partie) {
    if (finDePartie(partie)) {
        if (partie->joueur1->score > partie->joueur2->score) {
            printf("Joueur 1 vainqueur\n");
            return partie->joueur1;
        } else if (partie->joueur2->score > partie->joueur1->score) {
            printf("Joueur 2 vainqueur\n");
            return partie->joueur2;
        } else {
            printf("Ikiwake\n");
            return NULL;
        }
    }
    return NULL;
}

Partie* init_partie(const char* pseudo1, const char * pseudo2){
    Partie *partie_en_cours;
    Plateau *board = (Plateau *)malloc(sizeof(Plateau));
    if (board == NULL) {
        printf("Erreur d'allocation mémoire\n");
    }
    
    init_board(board);
    Joueur *joueura = (Joueur *)malloc(sizeof(Joueur));
    Joueur *joueurb = (Joueur *)malloc(sizeof(Joueur));
    init_joueur(joueura, pseudo1);
    init_joueur(joueurb, pseudo2);
    partie_en_cours = (struct Partie *)malloc(sizeof(Partie));
    partie_en_cours->plateau = board;
    partie_en_cours->joueur1 = joueura;
    partie_en_cours->joueur2 = joueurb;
    partie_en_cours->tour_sans_prise_consecutif = 0;

    partie_en_cours->joueur_actuel = tirage_au_sort(partie_en_cours);

    return partie_en_cours;
}
/*void lancer_partie(Partie partie_en_cours,int choix){

    while (!finDePartie(partie_en_cours)) {
        //afficher_plateau(board);
        //printf("Tour de %s (score : %d)\n", partie_en_cours->joueur_actuel->pseudo, partie_en_cours->joueur_actuel->score);
        
        /*int choix;
        printf("Choisissez une case : ");
        scanf("%d", &choix);

        if (choix < 1 || choix > 12) {
            printf("Choix invalide. Veuillez choisir une case entre 1 et 12.\n");
            continue;
        }

        if ((partie_en_cours->joueur_actuel == partie_en_cours->joueur1 && (choix < 1 || choix > 6)) ||
        (partie_en_cours->joueur_actuel == partie_en_cours->joueur2 && (choix < 7 || choix > 12))) {
            printf("Choix invalide. Vous devez choisir une case de votre propre camp.\n");
            continue;
        }

        if (!deplacement(choix - 1, board, partie_en_cours->joueur_actuel)) {
            printf("Déplacement non valide, choisissez une autre case.\n");
            continue;
        }

        partie_en_cours->joueur_actuel = (partie_en_cours->joueur_actuel == partie_en_cours->joueur1) ? partie_en_cours->joueur2 : partie_en_cours->joueur1;
    }

    Joueur *gagnant = vainqueur(partie_en_cours);
    if (gagnant != NULL) {
        printf("Le gagnant est %s avec %d points!\n", gagnant->pseudo, gagnant->score);
    }

}*/

void end_partie(Partie* partie){
    free(partie->plateau);
    free(partie->joueur1);
    free(partie->joueur2);
    free(partie);
}

void sauvegarder_partie(const char *nomFichier, Partie *partie) {
    FILE *fichier;
    int fichierExiste = (access(nomFichier, F_OK) == 0); // Vérifie si le fichier existe déjà

    // Ouvre en mode ajout (a), crée le fichier s'il n'existe pas
    fichier = fopen(nomFichier, "a");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Si le fichier n'existe pas, écrire les en-têtes
    if (!fichierExiste) {
        fprintf(fichier, "PartieID,Case1,Case2,Case3,Case4,Case5,Case6,Case7,Case8,Case9,Case10,Case11,Case12,\
        ScoreJoueur1,ScoreJoueur2,Joueur1,Joueur2,JoueurTour\n");
    }
    // Écrire les données
    for (int i = 0; i < 12; i++) {
        fprintf(fichier, "%d,", partie->plateau->plateau[i]);
    }
    fprintf(fichier, "%d,%d,%s,%s,%s\n",
            partie->joueur1->score,
            partie->joueur2->score,
            partie->joueur1->pseudo,
            partie->joueur2->pseudo,
            partie->joueur_actuel->pseudo);

    fclose(fichier);
    printf("Partie entre %s et %s sauvegardée dans %s\n",partie->joueur1->pseudo,partie->joueur2->pseudo, nomFichier);
}
