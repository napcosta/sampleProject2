/* 
 * My File System Interface
 * 
 * test-myfdround.c
 *
 * Tests the my file system interface:
 * - test if the file descriptors are being reused correctly
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myfs.h>
#include "tfuncs.h"


#define F_NAME "/f-fdround.txt"
#define F_SIZE (1024)

// NUM_OPEN_CLOSE > max number of file descriptors!
#define NUM_OPEN_CLOSE 40


int main(int argc, char **argv)
{
   int fd, num;

   // mandatory to init the myfs layer
   my_init_lib();


   // create the empty file
   fd = my_open(F_NAME,O_CREATE);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }

   // fill in a buffer with random binary data
   char data[F_SIZE];
   gen_data_rand_bin(data,F_SIZE);
   
   // write the data to the file
   num = my_write(fd, data, sizeof(data));
   if (num < sizeof(data)) {
      printf("[test] error writing data to file.\n");
      return -1;
   }

   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }


   /*
    * open and close the file several times to see if fd is reused
    */

   for (int i = 0; i < NUM_OPEN_CLOSE; i++) {
   
      // reopen the file
      fd = my_open(F_NAME,0);
      if (fd < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }

      // close the file
      if (my_close(fd) < 0) {
         printf("[test] error closing file.\n");
         return -1;
      }

      printf("[test] opened and closed [fd=%d]\n",fd);
   }
   

   /*
    * check the file integrity
    */
 
   
   // reopen file
   fd = my_open(F_NAME,0);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }
   
   // read file contents
   char buffer[F_SIZE];
   num = my_read(fd,buffer,sizeof(buffer));
   if (num != F_SIZE) {
      printf("[test] file size differ from the expected.\n");
      return -1;
   }
   
   // compare contents
   if (memcmp(data,buffer,F_SIZE) != 0) {
      printf("[test] file contents differ from data.\n");
      return -1;
   }
   
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }

   printf("[test] test passed.\n");
   return 0;
}
