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
bool famine(Partie *partie) {
    
    int debut_adversaire = (partie->joueur1 == partie->joueur_actuel) ? 0 : 6;
    int fin_adversaire = debut_adversaire + 6;
    int debut_joueur = (debut_adversaire == 0) ? 6 : 0;
    int fin_joueur = debut_joueur + 6;

    
    bool camp_vide = true;
    for (int i = debut_joueur; i < fin_joueur; i++) {
        if (partie->plateau->plateau[i] > 0) {
            camp_vide = false;
            break;
        }
    }

    if (!camp_vide) {
        return false;
    }

    // Vérifier si l'adversaire peut nourrir le joueur en famine
    for (int i = debut_adversaire; i < fin_adversaire; i++) {
        int graines = partie->plateau->plateau[i];
        if (graines > 0) {
            int positionFinale = (i + graines) % 12;

            // Vérifie si la graine finit dans le camp du joueur en famine
            if (positionFinale >= debut_joueur && positionFinale < fin_joueur) {
                return false; 
            }
        }
    }

  
    printf("Famine détectée\n");
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
    free(&(partie->plateau));
    free(&(partie->joueur1));
    free(&(partie->joueur2));
    free(partie);
}


void sauvegarder_partie(const char *nomFichier, Partie *partie, int emplacement) {
    FILE *fichier;
    char lignes[20][1024]; // pour stocker les lignes que l'on ne modifie pas
    int fichierExiste = (access(nomFichier, F_OK) == 0);

    // Vérification de l'emplacement valide
    if (emplacement < 0 || emplacement >= 20) {
        fprintf(stderr, "Erreur : emplacement invalide. Doit être entre 0 et 19.\n");
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier en lecture/écriture
    fichier = fopen(nomFichier, fichierExiste ? "r+" : "w+");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Si le fichier n'existe pas, initialiser avec une ligne d'en-têtes
    if (!fichierExiste) {
        fprintf(fichier, "PartieID,Plateau1,Plateau2,Plateau3,Plateau4,Plateau5,Plateau6,Plateau7,Plateau8,Plateau9,Plateau10,Plateau11,Plateau12,ScoreJoueur1,ScoreJoueur2,Joueur1,Joueur2,JoueurTour\n");
        // Initialiser 20 lignes vides pour les parties
        for (int i = 0; i < 20; i++) {
            fprintf(fichier, "%d,,,,,,,,,,,,,,,,,\n", i + 1);
        }
        fflush(fichier); // Assure l'écriture dans le fichier
    }

    // Lire les 20 lignes existantes dans le fichier
    rewind(fichier); // Retour au début du fichier
    fgets(lignes[0], sizeof(lignes[0]), fichier); // Ignorer l'en-tête
    for (int i = 0; i < 20; i++) {
        if (fgets(lignes[i], sizeof(lignes[i]), fichier) == NULL) {
            strcpy(lignes[i], "\n"); // Remplir avec une ligne vide si manquante
        }
    }

    // Créer la nouvelle ligne correspondant à la partie
    char nouvelleLigne[1024];
    snprintf(nouvelleLigne, sizeof(nouvelleLigne), 
             "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%s,%s\n",
             emplacement + 1, // PartieID
             partie->plateau->plateau[0], partie->plateau->plateau[1], partie->plateau->plateau[2],
             partie->plateau->plateau[3], partie->plateau->plateau[4], partie->plateau->plateau[5],
             partie->plateau->plateau[6], partie->plateau->plateau[7], partie->plateau->plateau[8],
             partie->plateau->plateau[9], partie->plateau->plateau[10], partie->plateau->plateau[11],
             partie->joueur1->score, partie->joueur2->score,
             partie->joueur1->pseudo, partie->joueur2->pseudo, partie->joueur_actuel->pseudo);

    // Remplacer la ligne correspondante
    strncpy(lignes[emplacement], nouvelleLigne, sizeof(lignes[emplacement]) - 1);

    // Réécrire tout le fichier
    rewind(fichier);
    fprintf(fichier, "PartieID,Plateau1,Plateau2,Plateau3,Plateau4,Plateau5,Plateau6,Plateau7,Plateau8,Plateau9,Plateau10,Plateau11,Plateau12,ScoreJoueur1,ScoreJoueur2,Joueur1,Joueur2,JoueurTour\n");
    for (int i = 0; i < 20; i++) {
        fputs(lignes[i], fichier);
    }

    fclose(fichier);
    printf("Partie entre %s et %s sauvegardée dans %s (emplacement %d)\n",
           partie->joueur1->pseudo,
           partie->joueur2->pseudo,
           nomFichier,
           emplacement);
}


void charger_parties_csv(const char *nomFichier, Partie *parties[]) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char ligne[1024];
    int ligneCourante = 0;
    int id;

    // Ignorer la première ligne d'en-tête
    if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
        perror("Erreur lors de la lecture de l'en-tête");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        

        Partie *partie = malloc(sizeof(Partie));
        Plateau *plateau = malloc(sizeof(Plateau));
        Joueur *joueur1 = malloc(sizeof(Joueur));
        Joueur *joueur2 = malloc(sizeof(Joueur));
        Joueur *joueur_actuel = malloc(sizeof(Joueur));
        partie->plateau = plateau;
        partie->joueur1 = joueur1;
        partie->joueur2 = joueur2;
        int result = sscanf(ligne,
               "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%[^,],%[^,],%[^\n]",
               &id, &plateau->plateau[0], &plateau->plateau[1], &plateau->plateau[2],
               &plateau->plateau[3], &plateau->plateau[4], &plateau->plateau[5],
               &plateau->plateau[6], &plateau->plateau[7], &plateau->plateau[8],
               &plateau->plateau[9], &plateau->plateau[10], &plateau->plateau[11],
               &joueur1->score, &joueur2->score,
               joueur1->pseudo, joueur2->pseudo, joueur_actuel->pseudo);

        // Vérifier que les pseudos des joueurs sont valides
        if (result < 17 || strlen(joueur1->pseudo) == 0 || strlen(joueur2->pseudo) == 0) {
            // Si l'un des joueurs n'a pas de pseudo ou si la ligne est mal formatée, on ignore cette partie
            free(partie->plateau);
            free(partie->joueur1);
            free(partie->joueur2);
            free(partie->joueur_actuel);
            free(partie);
            continue; 
        }

        
        partie->joueur_actuel = strcmp(joueur1->pseudo, joueur_actuel->pseudo) == 0 ? joueur1 : joueur2;

        parties[ligneCourante] = partie;
        ligneCourante++;
    }

    fclose(fichier);
    printf("Parties chargées depuis sauvegarde.\n");
}


