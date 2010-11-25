/* 
 * My File System Interface
 * 
 * test-myopen
 *
 * Tests the my file system interface:
 * - open: open a file
 * - close: close a file
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the my file system interface
#include <myfs.h>



int main(int argc, char **argv)
{
   int fd;

   // mandatory to init the myfs layer
   my_init_lib();

   // create the empty file 'test.txt' in the root directory
   fd = my_open("/test.txt",O_CREATE);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }

   // check that the fila exists by reopening it without the O_CREATE flag
   fd = my_open("/test.txt",0);
   if (fd < 0) {
      printf("[test] unable to open existing.\n");
      return -1;
   }
    
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }

   printf("[test] end.\n");
   return 0;
}
