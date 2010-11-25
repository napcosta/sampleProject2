/* 
 * SNFS API Layer
 * 
 * test-ping.c
 *
 * Tests the ping dummy SNFS service just to
 * ensure that the server is alive and kicking
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the SNFS API interface
#include <snfs_api.h>


#ifndef CLIENT_SOCK
#define CLIENT_SOCK "/tmp/client.socket"
#endif

#ifndef SERVER_SOCK
#define SERVER_SOCK "/tmp/server.socket"
#endif


int main(int argc, char **argv)
{
   // initialize the SNFS API layer - mandatory
   if (snfs_init(CLIENT_SOCK,SERVER_SOCK) < 0) {
      printf("[test-ping] unable to initialize SNFS API.\n");
      return -1;
   } else {
      printf("[test-ping] SNFS API initialized.\n");
   }

   // invoke the ping service sending a hello string
   char inmsg[] = "PING to server!";
   char outmsg[64];
   if (snfs_ping(inmsg,strlen(inmsg)+1,outmsg,sizeof(outmsg)) != STAT_OK) {
      printf("[test-ping] error pinging server.\n");
      return -1;
   }

   // if sucessful prints the response sent by server
   printf("[test-ping] response = '%s'\n", outmsg);
   return 0;
}
