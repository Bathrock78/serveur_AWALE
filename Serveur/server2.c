#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include <string.h>

#include "server2.h"
#include "client2.h"
#include "partie.h"

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
   /* the index for the array */
   int actual = 0;
   int max = sock;
   int actual_partie = 0;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   Partie* parties_en_cours[20];
   int parties_dispos[20];
   for (int i; i<20; i++){
      parties_dispos[i] = 0;
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

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

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
            choisir_option(c);
            c.etat = MENU;
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
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
               }
               
               if (c > 0) {
                  if (clients[i].etat==MENU){
                     if (strcmp(buffer, "LISTE") == 0) {
                        char client_list[BUF_SIZE];
                        list_clients(clients, actual, client_list);
                        strncat(client_list,"\n", BUF_SIZE - strlen(client_list) - 1);
                        write_client(client.sock, client_list);
                        choisir_option(clients[i]);
                     } 
                     else if (strcmp(buffer, "PARTIE") == 0) {
                        strncpy(buffer, "ALEATOIRE ou CHOISIR adversaire ?", BUF_SIZE - 1);
                        write_client(client.sock, buffer);
                        read_client(clients[i].sock, buffer);
                        int found = 0;
                        if (strcmp(buffer, "ALEATOIRE") == 0) {
                           for (int j = 0; j < actual; j++) {
                              if (clients[j].etat == MENU && clients[j].sock != clients[i].sock) {
                                 found = 1;
                                 /*int choix = choix_partie(clients[i],clients[j], clients, actual,buffer);
                                 if(!choix){
                                    break;
                                 }*/
                                
                                Partie* partie = init_partie(clients[i].name, clients[j].name);
                                parties_en_cours[actual_partie] = partie;
                                clients[i].num_partie = actual_partie;
                                clients[j].num_partie = actual_partie;
                                partie->client_1 = i;
                                partie->client_2 = j;

                                actual_partie++;
                                sauvegarder_partie("Data/sauvegardes.csv",partie);
                                
                                if (!strcmp(partie->joueur_actuel->pseudo, clients[i].name)){
                                    strncpy(buffer, "C'est à toi de jouer !\n", BUF_SIZE - 1);
                                    write_client(clients[i].sock, buffer);
                                    strncpy(buffer, "L'adversaire commence!\n", BUF_SIZE - 1);
                                    write_client(clients[j].sock, buffer);
                                    clients[i].etat = PARTIE_TOUR;
                                    clients[j].etat = PARTIE_ATTENTE;
                                }
                                else{
                                    strncpy(buffer, "C'est à toi de jouer !\n", BUF_SIZE - 1);
                                    write_client(clients[j].sock, buffer);
                                    strncpy(buffer, "L'adversaire commence !\n", BUF_SIZE - 1);
                                    write_client(clients[i].sock, buffer);
                                    clients[j].etat = PARTIE_TOUR;
                                    clients[i].etat = PARTIE_ATTENTE;
                                }
                                afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->pseudo);
                                write_client(clients[i].sock, buffer_plateau);
                                write_client(clients[j].sock, buffer_plateau);
                                break; 

                              }
                           }
                           if (!found) {
                              strncpy(buffer, "Aucun joueur disponible pour une partie.\n", BUF_SIZE - 1);
                              write_client(client.sock, buffer);
                              choisir_option(clients[i]);
                           }
                           
                           break;
                           
                        }else if (strcmp(buffer, "CHOISIR") == 0) {
                           
                           char client_dispos_list[BUF_SIZE];
                           list_clients_dispos(clients[i], clients, actual, client_dispos_list);
                           strncat(buffer,"\n", BUF_SIZE - strlen(client_dispos_list) - 1);
                           write_client(client.sock, client_dispos_list);
                           read_client(clients[i].sock,buffer);
                           for(int j = 0; j < actual; j++){
                              if(!strcmp(clients[j].name,buffer)){
                                 choix_partie(clients[i],clients[j],clients,actual,buffer);
                                 found = 1;
                                 break;
                              }
                           }
                           if (!found) {
                                 strncpy(buffer, "Aucun joueur libre correspondant.\n", BUF_SIZE - 1);
                                 write_client(client.sock, buffer);
                                 choisir_option(clients[i]);
                              }
                           
                        }else{
                           write_client(clients[i].sock,"Choix indisponible, retou au menu\n");
                           choisir_option(clients[i]);
                        }
                     }else {
                        //send_message_to_all_clients(clients, client, actual, buffer, 0);
                        write_client(clients[i].sock,"Choix indisponible\n");
                        choisir_option(clients[i]);
                     }
                  }
                  else if (clients[i].etat == PARTIE_ATTENTE){
                     Partie* partie = parties_en_cours[clients[i].num_partie];
                     strncpy(buffer, "En attente coup adversaire !", BUF_SIZE - 1);
                     write_client(clients[i].sock, buffer);

                  }
                  else if (clients[i].etat == PARTIE_TOUR){
                     read_client(clients[i].sock,buffer);
                     int choix = atoi(buffer);
                     Partie* partie = parties_en_cours[clients[i].num_partie];
                     //Choix de la case à jouer
                     
                     if (choix < 1 || choix > 12) {
                        strncpy(buffer, "Choix invalide. Veuillez choisir une case entre 1 et 12.\n", BUF_SIZE - 1);
                        write_client(clients[i].sock, buffer);
                        break;
                        
                     }

                     if ((partie->joueur_actuel == partie->joueur1 && (choix < 1 || choix > 6)) ||
                     (partie->joueur_actuel == partie->joueur2 && (choix < 7 || choix > 12))) {
                        strncpy(buffer, "Choix invalide. Vous devez choisir une case de votre propre camp.\n", BUF_SIZE - 1);
                        write_client(clients[i].sock, buffer);
                        break;
                     }

                     if (!deplacement(choix - 1, partie->plateau, partie->joueur_actuel)) {
                        strncpy(buffer, "Déplacement non valide, choisissez une autre case.\n", BUF_SIZE - 1);
                        write_client(clients[i].sock, buffer);
                        break;
                     }
                     if(!finDePartie(partie)){
                        clients[i].etat = PARTIE_ATTENTE;
                        Client adv;
                        if (partie->client_1 == i){
                           adv = clients[partie->client_2];
                        }
                        else{
                           adv = clients[partie->client_1];
                        
                        }
                        adv.etat = PARTIE_TOUR;
                        

                        afficher_plateau(buffer_plateau, BUF_SIZE, partie->plateau, partie->joueur1->score, partie->joueur1->pseudo, partie->joueur2->score, partie->joueur2->score);
                        write_client(clients[i].sock, buffer_plateau);
                        write_client(adv.sock, buffer_plateau);
                        strncpy(buffer, "En attente coup adversaire !", BUF_SIZE - 1);
                        write_client(clients[i].sock, buffer);
                        strncpy(buffer, "A ton tour ! \n", BUF_SIZE - 1);
                        write_client(adv.sock, buffer);
                        partie->joueur_actuel = (partie->joueur_actuel == partie->joueur1) ? partie->joueur2 : partie->joueur1;
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
                        //memmov(partie)
                        clients[i].num_partie = -1;
                        clients[i].etat = MENU;
                        choisir_option(clients[i]);
                        
                     }

                     
                  }
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

int choix_partie(Client client_demandeur, Client client_repondeur, Client* clients, int actual, char* buffer){
   snprintf(buffer, BUF_SIZE, "Une partie est proposée avec %s. En attente d'acceptation\n", client_repondeur.name);
   write_client(client_demandeur.sock, buffer);
   snprintf(buffer, BUF_SIZE, "Une partie a été proposée par %s. ACCEPTER ou REFUSER ?\n", client_demandeur.name);
   write_client(client_repondeur.sock, buffer);
   read_client(client_repondeur.sock, buffer);
   if(strcmp(buffer, "ACCEPTER") == 0){
      client_demandeur.etat = PARTIE_TOUR;
      client_repondeur.etat = PARTIE_ATTENTE;
      snprintf(buffer, BUF_SIZE, "Partie avec %s acceptée. La partie va commencer...\n", client_repondeur.name);
      write_client(client_demandeur.sock, buffer);
      snprintf(buffer, BUF_SIZE, "Partie avec %s acceptée. La partie va commencer\n", client_demandeur.name);
      write_client(client_repondeur.sock, buffer);
      return 1;
   }else if(strcmp(buffer, "REFUSER") == 0){
      snprintf(buffer, BUF_SIZE, "Partie avec %s refusée.\n", client_repondeur.name);
      write_client(client_demandeur.sock, buffer);
      choisir_option(client_demandeur);

      snprintf(buffer, BUF_SIZE, "Partie avec %s refusée.\n", client_demandeur.name);
      write_client(client_repondeur.sock, buffer);
      choisir_option(client_repondeur);
      return 0;
   }else{
      choix_partie(client_demandeur, client_repondeur, clients, actual, buffer);
   }
}
void list_clients(Client *clients, int actual, char *buffer) {
    int i;
    strcpy(buffer, "Joueurs connectés :\n");
    for (i = 0; i < actual; i++) {
        strncat(buffer, clients[i].name, BUF_SIZE - strlen(buffer) - 1);
        strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
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
      strncpy(buffer,"\nBienvenue dans Awale !\nVeuillez choisir une option :\n1.Afficher tous les joueurs présents, tapez 'LISTE'\n2. Jouer à Awale, tapez PARTIE\n" , BUF_SIZE - 1);
      write_client(c.sock, buffer);
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