/* 
 * SNFS API Layer
 * 
 * test-defrag
 *
 * Tests the SNFS services:
 * - create: creates 2 files
 * - remove: removes the first file 
 * - defrag: defragments file system
 * - diskusage: dumps FS's occupied data blocks
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
   
   snfs_fhandle_t file_fh1;
   snfs_fhandle_t file_fh2;

   // invoke the 'create' service to create file 'f1' in root dir
      if (snfs_create(ROOT_FHANDLE,"f1",&file_fh1) != STAT_OK) {
      printf("[test] error creating a file in server.\n");
      return -1;
   }
   printf("[test] file created with file handle %d.\n",file_fh1);
   
  /*
    * do the 'write' fo file "f1"
    */
   
   // build a string of 'A' chars
   char data[DATA_SIZE];
   for (int i = 0; i < DATA_SIZE-1; data[i] = 'A', i++);
   data[DATA_SIZE-1] = '\0';
   
   // invoke the 'write' service to write to file 'f1'
   unsigned fsize;
   if (snfs_write(file_fh1,0,DATA_SIZE,data,&fsize) != STAT_OK) {
      printf("[test] error writing to file.\n");
      return -1;
   }
      
   // invoke the 'create' service to create file 'f2' in root dir
   if (snfs_create(ROOT_FHANDLE,"f2",&file_fh2) != STAT_OK) {
      printf("[test] error creating a file in server.\n");
      return -1;
   }
   printf("[test] file created with file handle %d.\n",file_fh2);
   
   /*
    * do the 'write' fo file "f2"
    */
   
   // build a string of 'B' chars
   for (int i = 0; i < DATA_SIZE-1; data[i] = 'B', i++);
   data[DATA_SIZE-1] = '\0';
   
   // invoke the 'write' service to write to file 'f2'
   if (snfs_write(file_fh2,0,DATA_SIZE,data,&fsize) != STAT_OK) {
      printf("[test] error writing to file.\n");
      return -1;
   }

   // test if all data was written
   if (fsize != DATA_SIZE) {
      printf("[test] error: sizes differ %d!=%d.\n",fsize,DATA_SIZE);
      return -1;
   }
   //removing file "f1" means that the first  data block (11th) of FS will be emptied as the following block
   //keeps storing file "f2" data (12th).   
   if (snfs_remove(ROOT_FHANDLE, "f1", &file_fh1) != STAT_OK) {
      printf("[test] error deleting file in server.\n");
      return -1;
   }   
   //defragmenting the FS
   if (snfs_defrag() != STAT_OK) {
      printf("[test] error defragmenting server's file system.\n");
      return -1;
   }   
   //Dumping FS diskusage
   if (snfs_diskusage() != STAT_OK) {
      printf("[test] error dumping server's file system.\n");
      return -1;
   }   
   
   printf("[test] File system was successfully defragmented only if occupied data block is \n      - the first one (the 11th) storing file 'f2' data\n    \n(Please check diskusage dumping on the server terminal).\n\n");

   return 0;
}
