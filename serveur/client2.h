#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef enum {
    MENU, 
    PARTIE,
} State;

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   State etat;
}Client;



#endif /* guard */
