#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef enum {
    MENU, 
    PARTIE_TOUR,
    PARTIE_ATTENTE,
} State;

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   State etat;
   int num_partie;
}Client;



#endif /* guard */
