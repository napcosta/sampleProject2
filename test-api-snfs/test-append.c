/* 
 * SNFS API Layer
 * 
 * test-append
 *
 * Tests the SNFS services:
 * - create: creates 2 files
 * - write: writes data to a file 
 * - append: appends one file to the end of another file
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

   // invoke the 'create' service to create file 'f1' in root dir
   snfs_fhandle_t file_fh;
   if (snfs_create(ROOT_FHANDLE,"f1",&file_fh) != STAT_OK) {
      printf("[test] error creating a file in server.\n");
      return -1;
   }
   printf("[test] file created with file handle %d.\n",file_fh);
   
   // invoke the 'create' service to create file 'f2' in root dir
   if (snfs_create(ROOT_FHANDLE,"f2",&file_fh) != STAT_OK) {
      printf("[test] error creating a file in server.\n");
      return -1;
   }
   printf("[test] file created with file handle %d.\n",file_fh);
   

   /*
    * do the 'write'
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

   // test if all data was written
   if (fsize != DATA_SIZE) {
      printf("[test] error: sizes differ %d!=%d.\n",fsize,DATA_SIZE);
      return -1;
   }
   
   //appending one file to another   
   if (snfs_append(ROOT_FHANDLE,"f1",ROOT_FHANDLE,"f2",&fsize) != STAT_OK) {
      printf("[test] error copying a file.\n");
      return -1;
   }
   
   printf("[test] file was successfully appended.\n");
   printf("[test] PASSED.\n");
   
   return 0;
}
