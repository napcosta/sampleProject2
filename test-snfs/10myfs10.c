/* 
 * My File System Interface
 * 
 * test-myfdmax
 *
 * Tests the my file system interface:
 * - ensures that there the max num of opened files is being checked
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myfs.h>
#include "tfuncs.h"


#ifndef MAX_NUM_FDS
#define MAX_NUM_FDS 10
#endif


#define NUM_FILES (MAX_NUM_FDS+1)


int main(int argc, char **argv)
{
   int fd;
   char fname[1024];

   my_init_lib();

   for (int i = 0; i < NUM_FILES; i++) {
      snprintf(fname,sizeof(fname),"/x-%d",i);

      // create an empty file
      fd = my_open(fname,O_CREATE);
      if (fd < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }

      // close the file
      if (my_close(fd) < 0) {
         printf("[test] error closing file.\n");
         return -1;
      }
   }

   for (int i = 0; i < NUM_FILES; i++) {
      snprintf(fname,sizeof(fname),"/x-%d",i);

      // just open it
      fd = my_open(fname,0);
      if (fd < 0) {
         if (i == MAX_NUM_FDS) {
            printf("[test] passed - max opened files %d reached.\n",i);
            return 0;
         } else {
            printf("[test] unable to open file.\n");
            return -1;
         }
      }
   }

   printf("[test] did not passed.\n");
   return 0;
}
