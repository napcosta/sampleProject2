/* 
 * SNFS API Layer
 * 
 * test-dumpcache
 *
 * Tests the SNFS services:
 * - create: creates 2 files
 * - dumpcache: dumps cache of blocks content on server side
 * 
 * Atention: in current release, only the root directory exists
 * which corresponds to the file handler 1.
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

#define ROOT_FHANDLE 1

#define DATA_SIZE 512


int main(int argc, char **argv)
{
   // initialize the SNFS API layer
   if (snfs_init(CLIENT_SOCK,SERVER_SOCK) < 0) {
      printf("[test] unable to initialize SNFS API.\n");
      return -1;
   } else {
      printf("[test] SNFS API initialized.\n");
   }
   
   snfs_fhandle_t file_fh;

   // invoke the 'create' service to create file 'f1' in root dir
   if (snfs_create(ROOT_FHANDLE,"f1",&file_fh) != STAT_OK) {
      printf("[test] error creating a file in server.\n");
      return -1;
   }
   printf("[test] file created with file handle %d.\n",file_fh);
   
  /*
    * do the 'write' fo file "f1"
    */
   
   // build a string of 'A' chars
   char data[DATA_SIZE];
   for (int i = 0; i < DATA_SIZE-1; data[i] = 'A', i++);
   data[DATA_SIZE-1] = '\0';
   
   // invoke the 'write' service to write to file 'f1'
   unsigned fsize;
   if (snfs_write(file_fh,0,DATA_SIZE,data,&fsize) != STAT_OK) {
      printf("[test] error writing to file.\n");
      return -1;
   }
      
   //Dumping Cache of Blocks's entries on server side
   if (snfs_dumpcache() != STAT_OK) {
      printf("[test] error dumping the entries of the cache of blocks .\n");
      return -1;
   }   
   
   printf("\n[test] Cache of blocks's dumping is successfull if block 11 is the only one cached.\n    \n(Please check check dumping list on the server terminal).\n\n");

   return 0;
}

