#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "server2.h"
#include "client2.h"
#include "partie.h"

#define TIME_LIMIT 60 //temps maximal pour jouer un coup
#define MAX_PARTIES 20

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   char buffer_plateau[BUF_SIZE];
   bool avertissements_envoyes[MAX_PARTIES] = { false };
   /* the index for the array */
   int actual = 0;
   int max = sock;
   int actual_partie = 0;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   Partie** parties_en_cours = malloc(20 * sizeof(Partie*));
   if (access("Data/sauvegardes.csv", F_OK) == 0){
      charger_parties_csv("Data/sauvegardes.csv", parties_en_cours);
   }

   fd_set rdfs;

   while(1)
   {
      

      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      // Appeler select() avec un timeout pour éviter un blocage long
         struct timeval timeout = {0, 500000}; // 0.5 seconde
         if (select(max + 1, &rdfs, NULL, NULL, &timeout) < 0) {
            perror("select()");
            continue;
         }
         verifier_temps(parties_en_cours, clients, MAX_PARTIES, actual,avertissements_envoyes);


      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }
         

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         
         strncpy(c.name, buffer, BUF_SIZE - 1);
         int verif = 0;
         for (int i =0; i<actual; i++){
            if(!strcmp(clients[i].name, c.name)){
               verif = 1;
            }
         }

         
         

         if (verif == 0){
            // on veut verifier que le client n'est pas déjà dans une partie
            int verif_partie = 0;
            for (int i = 0; i < 20; i++){
               if (parties_en_cours[i] != NULL){
                  if (!strcmp(parties_en_cours[i]->joueur1->pseudo, c.name) || !strcmp(parties_en_cours[i]->joueur2->pseudo, c.name)){
                     if(!strcmp(parties_en_cours[i]->joueur1->pseudo, c.name)){
                        c.adversaire = parties_en_cours[i]->client_2;
                     }else{
                        c.adversaire = parties_en_cours[i]->client_1;
                     }
                     
                     strncpy(buffer, "Vous êtes déjà dans une partie en cours !\n", BUF_SIZE - 1);
                     write_client(c.sock, buffer);
                     // on regarde si c'est son tour ou pas
                     if (!strcmp(parties_en_cours[i]->joueur_actuel->pseudo, c.name)){
                        strncpy(buffer, "C'est à vous de jouer !\n", BUF_SIZE - 1);
                        afficher_plateau(buffer_plateau, BUF_SIZE, parties_en_cours[i]->plateau, parties_en_cours[i]->joueur1->score, 
                        parties_en_cours[i]->joueur1->pseudo, parties_en_cours[i]->joueur2->score, parties_en_cours[i]->joueur2->pseudo,c.name);
                        write_client(c.sock, buffer_plateau);
                        c.etat = PARTIE_TOUR;
                        c.adversaire->etat = PARTIE_ATTENTE;
                     }
                     else{
                        strncpy(buffer, "C'est à votre adversaire de jouer !\n", BUF_SIZE - 1);
                        c.etat = PARTIE_ATTENTE;
                        c.adversaire->etat = PARTIE_TOUR;
                     }
                     write_client(c.sock, buffer);
                     verif_partie = 1;
                     break;
                  }
               }
            }
            if(!verif_partie){
               choisir_option(c);
               c.etat = MENU;
            }

            clients[actual] = c;
            actual++;
         }
         else{
            strncpy(buffer, "Dommage pseudo deja pris !\n", BUF_SIZE - 1);
            write_client(c.sock, buffer);
            end_connection(c.sock);
         }
         
      }
      else
      {
         
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
               }
               
               if (c > 0) {
                  switch(clients[i].etat){
                     case MENU:
                        if (strcmp(buffer, "LISTEC") == 0) {
                           char client_list[BUF_SIZE];
                           list_clients(clients, actual, client_list);
                           strncat(client_list,"\n", BUF_SIZE - strlen(client_list) - 1);
                           write_client(client.sock, client_list);
                           choisir_option(clients[i]);
                        }else if (strcmp(buffer, "LISTEP") == 0) {
                           char partie_list[BUF_SIZE];
                           list_parties(parties_en_cours, actual, partie_list);
                           strncat(partie_list,"\n", BUF_SIZE - strlen(partie_list) - 1);
                           write_client(client.sock, partie_list);
                           choisir_option(clients[i]);
                        }else if (strcmp(buffer, "PARTIE") == 0) {
                           strncpy(buffer, "ALEATOIRE ou CHOISIR adversaire ?", BUF_SIZE - 1);
                           write_client(client.sock, buffer);
                           clients[i].etat = MENU_TYPE_PARTIE;
                        }else{
                           strncpy(buffer, "Choix indisponible", BUF_SIZE - 1);
                           write_client(client.sock, buffer);
                        }
                        break;

                     case MENU_TYPE_PARTIE:
                        int found = 0;
                        if (strcmp(buffer, "ALEATOIRE") == 0) {
                           for (int j = 0; j < actual; j++) {
                              if (clients[j].etat == MENU && clients[j].sock != clients[i].sock) {
                                 found = 1;
                                 clients[i].adversaire = &clients[j];
                                 clients[i].adversaire->adversaire = &clients[i];
                                 choix_partie(clients[i],clients[j]);
                                 clients[i].adversaire->etat = REPONDRE_DEMANDE_PARTIE; 
                                break; 

                              }
                           }
                           if (!found) {
                              strncpy(buffer, "Aucun joueur disponible pour une partie.\n", BUF_SIZE - 1);
                              write_client(client.sock, buffer);
                              choisir_option(clients[i]);
                              clients[i].etat = MENU;
                           }
                           
                        }else if (strcmp(buffer, "CHOISIR") == 0) {
                           char client_dispos_list[BUF_SIZE];
                           list_clients_dispos(clients[i], clients, actual, client_dispos_list);
                           strncat(buffer,"\n", BUF_SIZE - strlen(client_dispos_list) - 1);
                           write_client(client.sock, client_dispos_list);
                           clients[i].etat = MENU_CHOIX_ADVERSAIRE;
                        }

                        break;

                     case MENU_CHOIX_ADVERSAIRE:
                        for(int j = 0; j < actual; j++){
                           if(!strcmp(clients[j].name,buffer)){
                              found = 1;
                              clients[i].adversaire = &clients[j];
                              clients[i].adversaire->adversaire = &clients[i];
                              Partie* partie = init_partie(clients[i].name, clients[j].name);
                              partie->client_2 = &clients[j];
                              partie->client_1 = &clients[i];
                              choix_partie(clients[i],clients[j]);
                              clients[j].etat = REPONDRE_DEMANDE_PARTIE;
                              break;
                           }
                        }
                        if (!found) {
                           strncpy(buffer, "Aucun joueur libre correspondant.\n", BUF_SIZE - 1);
                           write_client(client.sock, buffer);
                           choisir_option(clients[i]);
                           clients[i].etat = MENU;
                        }
                        break;
                        
                     case REPONDRE_DEMANDE_PARTIE:

                        if(strcmp(buffer, "ACCEPTER") == 0){
                           snprintf(buffer, BUF_SIZE, "Partie avec %s acceptée. La partie va commencer\n", clients[i].adversaire->name);
                           write_client(clients[i].sock, buffer);
                           snprintf(buffer, BUF_SIZE, "Partie avec %s acceptée. La partie va commencer\n", clients[i].name);
                           write_client(clients[i].adversaire->sock, buffer);
                           Partie* partie = init_partie(clients[i].adversaire->name,clients[i].name);
                           partie->client_2 = &clients[i].adversaire;
                           partie->client_1 = &clients[i];

                           //on parcourt les parties en cours pour trouver une place libre
                           for (int j = 0; j < 20; j++){
                              if (parties_en_cours[j] == NULL){
                                 parties_en_cours[j] = partie;
                                 clients[i].num_partie = j;
                                 clients[i].adversaire->num_partie = j;
                                 break;
                              }
                           }
                           actual_partie++;
                           
                           if (!strcmp(partie->joueur_actuel->pseudo, clients[i].name)){
                              strncpy(buffer, "C'est à toi de jouer !", BUF_SIZE - 1);
                              write_client(clients[i].sock, buffer);
                              strncpy(buffer, "L'adversaire commence!", BUF_SIZE - 1);
                              write_client(clients[i].adversaire->sock, buffer);
                              clients[i].etat = PARTIE_TOUR;
                              clients[i].adversaire->etat = PARTIE_ATTENTE;
                           }
                           else{
                              strncpy(buffer, "C'est à toi de jouer !", BUF_SIZE - 1);
                              write_client(clients[i].adversaire->sock, buffer);
                              strncpy(buffer, "L'adversaire commence !", BUF_SIZE - 1);
                              write_client(clients[i].sock, buffer);
                              
                              clients[i].adversaire->etat = PARTIE_TOUR;
                              clients[i].etat = PARTIE_ATTENTE;
                           }
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].name);
                           write_client(clients[i].sock, buffer_plateau);
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].adversaire->name);
                           write_client(clients[i].adversaire->sock, buffer_plateau);
                           partie->debut_tour = time(NULL);
                        }else if(strcmp(buffer, "REFUSER") == 0){
                           snprintf(buffer, BUF_SIZE, "Partie avec %s refusée.\n", clients[i].adversaire->name);
                           write_client(clients[i].sock, buffer);
                           choisir_option(clients[i]);

                           snprintf(buffer, BUF_SIZE, "Partie avec %s refusée.\n", clients[i].name);
                           write_client(clients[i].adversaire->sock, buffer);
                           choisir_option(*(clients[i].adversaire));
                           clients[i].adversaire->etat = MENU;
                           clients[i].etat = MENU;
                        }else{
                           strncpy(buffer, "Choix invalide. ACCEPTER ou REFUSER ?", BUF_SIZE - 1);
                           write_client(clients[i].sock, buffer);                           
                        }

                        break;

                     case PARTIE_ATTENTE:
                        Partie* partie = parties_en_cours[clients[i].num_partie];
                        strncpy(buffer, "En attente coup adversaire !", BUF_SIZE - 1);
                        write_client(clients[i].sock, buffer);
                        break;

                     case PARTIE_TOUR:
                        int choix = atoi(buffer);
                        partie = parties_en_cours[clients[i].num_partie];
                        partie->debut_tour = time(NULL);

                        //Choix de la case à jouer
                        if (choix < 1 || choix > 12) {
                           strncpy(buffer, "Choix invalide. Veuillez choisir une case entre 1 et 12.\n", BUF_SIZE - 1);
                           write_client(clients[i].sock, buffer);
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].name);
                           write_client(clients[i].sock, buffer);
                           break;
                           
                        }

                        if ((partie->joueur_actuel == partie->joueur1 && (choix < 1 || choix > 6)) ||
                        (partie->joueur_actuel == partie->joueur2 && (choix < 7 || choix > 12))) {
                           strncpy(buffer, "Choix invalide. Vous devez choisir une case de votre propre camp.\n", BUF_SIZE - 1);
                           write_client(clients[i].sock, buffer);
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].name);
                           write_client(clients[i].sock, buffer_plateau);
                           break;
                        }

                        if (!deplacement(choix - 1, partie->plateau, partie->joueur_actuel)) {
                           strncpy(buffer, "Déplacement non valide, choisissez une autre case.\n", BUF_SIZE - 1);
                           write_client(clients[i].sock, buffer);
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].name);
                           write_client(clients[i].sock, buffer_plateau);
                           break;
                        }
                        if(!finDePartie(partie)){
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].name);
                           write_client(clients[i].sock, buffer_plateau);
                           afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo,clients[i].adversaire->name);
                           write_client(clients[i].adversaire->sock, buffer_plateau);
                           strncpy(buffer, "En attente coup adversaire !\n", BUF_SIZE - 1);
                           write_client(clients[i].sock, buffer);
                           strncpy(buffer, "A ton tour ! \n", BUF_SIZE - 1);
                           write_client(clients[i].adversaire->sock, buffer);
                           snprintf(buffer, BUF_SIZE, "Adversaire: %s \n", clients[i].adversaire->name);
                           write_client(clients[i].sock, buffer);
                           partie->joueur_actuel = (partie->joueur_actuel == partie->joueur1) ? partie->joueur2 : partie->joueur1;
                           clients[i].etat = PARTIE_ATTENTE;
                           clients[i].adversaire->etat = PARTIE_TOUR;
                           sauvegarder_partie("Data/sauvegardes.csv",partie, actual_partie);
                        }else{
                           Joueur *gagnant = vainqueur(partie);
                           if (gagnant != NULL) {
                              snprintf(buffer, BUF_SIZE, "Le gagnant est %s avec %d points!\n", gagnant->pseudo,gagnant->score); 
                              write_client(clients[i].sock, buffer);
                           }
                           else{
                              strncpy(buffer, "IKIWAKE\n", BUF_SIZE - 1);
                              write_client(clients[i].sock, buffer);
                           }   
                           end_partie(&partie);
                           parties_en_cours[clients[i].num_partie] = NULL;
                           actual_partie --;
                           clients[i].num_partie = -1;
                           clients[i].adversaire->num_partie = -1;
                           choisir_option(*(clients[i].adversaire));
                           choisir_option(clients[i]);
                           clients[i].etat = MENU;
                           clients[i].adversaire->etat = MENU;
                           
                        }
                        break;
                     
                     
                     

                     default:
                        printf("État inconnu pour le client (name: %d).\n", clients[i].name);
                        break;

                  }
               }
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

void choix_partie(Client client_demandeur, Client client_repondeur){
   char buffer[BUF_SIZE];
   snprintf(buffer, BUF_SIZE, "Une partie est proposée avec %s. En attente d'acceptation\n", client_repondeur.name);
   write_client(client_demandeur.sock, buffer);
   snprintf(buffer, BUF_SIZE, "Une partie a été proposée par %s. ACCEPTER ou REFUSER ?\n", client_demandeur.name);
   write_client(client_repondeur.sock, buffer); 
}
void list_clients(Client *clients, int actual, char *buffer) {
    int i;
    strcpy(buffer, "Joueurs connectés :\n");
    for (i = 0; i < actual; i++) {
        strncat(buffer, clients[i].name, BUF_SIZE - strlen(buffer) - 1);
        strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
    }
}

void list_parties(Partie **parties_en_cours, int actual, char *buffer) {
    int i;
    strncpy(buffer, "Parties en cours :\n", BUF_SIZE - 1);

    for (i = 0; i < 20; i++) {
        if (parties_en_cours[i] != NULL) {
            char ligne[256];
            snprintf(ligne, sizeof(ligne), 
                     "Partie : %s vs %s | Score : %d - %d \n",
                     parties_en_cours[i]->joueur1->pseudo,
                     parties_en_cours[i]->joueur2->pseudo,
                     parties_en_cours[i]->joueur1->score,
                     parties_en_cours[i]->joueur2->score);
            strncat(buffer, ligne, BUF_SIZE - strlen(buffer) - 1);
        }
    }
    if (strlen(buffer) == strlen("Parties en cours :\n")) {
        strncat(buffer, "Aucune partie en cours.\n", BUF_SIZE - strlen(buffer) - 1);
    }
}


void list_clients_dispos(Client c, Client *clients, int actual, char *buffer) {
    int i;
    strcpy(buffer, "Choisissez votre adversaire :\n");
    for (i = 0; i < actual; i++) {
      if (clients[i].etat == MENU && strcmp(clients[i].name,c.name)){
        strncat(buffer, clients[i].name, BUF_SIZE - strlen(buffer) - 1);
        strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
      }
    }
}


void choisir_option(Client c){
   char buffer[BUF_SIZE];
   strncpy(buffer,"\nBienvenue dans Awale !\nVeuillez choisir une option :\n1.Afficher tous les joueurs présents, tapez 'LISTEC'\n2. Jouer à Awale, tapez PARTIE\n3.Afficher les parties en cours, tapez 'LISTEP'\n" , BUF_SIZE - 1);
   write_client(c.sock, buffer);
}

void verifier_temps(Partie *parties[], Client clients[], int nb_parties, int nb_clients, bool avertissements_envoyes[]) {
   char buffer[BUF_SIZE];
    time_t maintenant = time(NULL);

    for (int p = 0; p < nb_parties; p++) {
        Partie *partie = parties[p];
        if (partie == NULL) continue;

        time_t temps_ecoule = difftime(maintenant, partie->debut_tour);

        if (temps_ecoule >= TIME_LIMIT - 20 && temps_ecoule < TIME_LIMIT) {
            // Envoyer un avertissement à l'utilisateur
            if (!avertissements_envoyes[p]){
               for (int c = 0; c < nb_clients; c++) {
                  if (clients[c].num_partie == p && clients[c].etat == PARTIE_TOUR) {
                     snprintf(buffer, BUF_SIZE, "Attention ! Il vous reste 20 secondes pour jouer.\n");
                     write_client(clients[c].sock, buffer);
                  }
               }
               avertissements_envoyes[p] = true;
            }
            
        } else if (temps_ecoule >= TIME_LIMIT) {
            // Temps écoulé, le joueur perd la partie
            Joueur *perdant = partie->joueur_actuel;
            Joueur *gagnant = (perdant == partie->joueur1) ? partie->joueur2 : partie->joueur1;

            snprintf(buffer, BUF_SIZE, "Temps écoulé ! %s a perdu la partie.\n", perdant->pseudo);
            for (int c = 0; c < nb_clients; c++) {
                if (clients[c].num_partie == p) {
                    write_client(clients[c].sock, buffer);
                }
            }

            // Terminer la partie
            end_partie(&parties[p]);

            for (int c = 0; c < nb_clients; c++) {
                if (clients[c].num_partie == p) {
                    clients[c].num_partie = -1;
                    choisir_option(clients[c]);
                    clients[c].etat = MENU;
                }
            }
            avertissements_envoyes[p] = false;
        }else{
            if (temps_ecoule < TIME_LIMIT - 10) {
                avertissements_envoyes[p] = false;
            }
        }
    }
}


static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}


static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
