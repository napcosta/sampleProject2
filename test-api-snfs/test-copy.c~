/* 
 * SNFS API Layer
 * 
 * test-copy
 *
 * Tests the SNFS services:
 * - create: creates a file
 * - write: writes data to a file 
 * - copy: makes a file copy
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
	

	snfs_fhandle_t dir_fh;
	if (snfs_mkdir(ROOT_FHANDLE, "dir1", &dir_fh) != STAT_OK) {
	   printf("[test] error creating dir in server.\n");
	   return -1;
	}


   // invoke the 'create' service to create file 'f1' in root dir
   snfs_fhandle_t file_fh;
   if (snfs_create(dir_fh,"f1",&file_fh) != STAT_OK) {
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

   //copying "f1" to "f2"
   if (snfs_copy(ROOT_FHANDLE,"dir1",ROOT_FHANDLE,"dir2",&dir_fh) != STAT_OK) {
      printf("[test] error copying a file.\n");
      return -1;
   }
  
   printf("[test] copy successful to a file with file handle %d.\n",file_fh);
   printf("[test] PASSED.\n");
   
   return 0;
}
