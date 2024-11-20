#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef enum {
    MENU,
    MENU_TYPE_PARTIE,
    MENU_CHOIX_ADVERSAIRE,
    REPONDRE_DEMANDE_PARTIE,
    ATT_REPONSE_PARTIE,
    PARTIE_TOUR,
    PARTIE_ATTENTE,
} State;

typedef struct Client
{
   SOCKET sock;
   char name[BUF_SIZE];
   State etat;
   int num_partie;
}Client;



#endif /* guard */
