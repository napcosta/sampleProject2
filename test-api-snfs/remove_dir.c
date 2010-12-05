/* 
 * SNFS API Layer
 * 
 * test-remove
 *
 * Tests the SNFS services:
 * - remove: removes a file or an empty directory from the file system
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
   snfs_fhandle_t fh;
   snfs_fhandle_t fh2;

	if (snfs_mkdir(ROOT_FHANDLE,"dir1",&fh) != STAT_OK) {
		printf("[test] error creating a dir in server.\n");
		return -1;
   	}

   if (snfs_create(fh,"f1",&file_fh) != STAT_OK) {
      printf("[test] error creating a file in server.\n");
      return -1;
   }

	if (snfs_create(fh, "d2", &fh2) != STAT_OK) {
		printf("[test] error creating dir2 in server\n");
		return -1;
	}


   printf("[test] file created with file handle %d.\n",file_fh);
   


   //removing file "f1" from root firectory
   if (snfs_remove(ROOT_FHANDLE, "dir1", &fh) != STAT_OK) {
      printf("[test] error deleting file in server.\n");
      return -1;
   }   

   printf("[test] file was successfully deleted.\n");
   printf("[test] PASSED.\n");
   
   return 0;
}
