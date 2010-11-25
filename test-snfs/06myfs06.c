/* 
 * My File System Interface
 * 
 * test-myfilebin.c
 *
 * Tests the my file system interface:
 * - tests if data is being manipulated as binary data and not as
 *   strings. This is done by writing a binary file with zeros in it.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myfs.h>
#include "tfuncs.h"

#define F_NAME "/t-bin.txt"


int main(int argc, char **argv)
{
   int fd, num;

   // mandatory to init the myfs layer
   my_init_lib();


   // create a file
   fd = my_open(F_NAME,O_CREATE);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }

   // fill in a buffer with binary data with zeros
   char data[2*1024];
   gen_data_rand_zbin(data,sizeof(data));

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
    * check that the file is not corrupted
    */
 
   
   // reopen file
   fd = my_open(F_NAME,0);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }

   // read and compare file contents with written data
   char buffer[512];
   int read = 0;
   while (1) {
      num = my_read(fd,buffer,sizeof(buffer));
      if (num < 0) {
         printf("[test] error reading data from file.\n");
         return -1;
      }
      if (num == 0) {
         break;
      }
      if (memcmp(&data[read],buffer,num) != 0) {
         printf("[test] file contents differ from data.\n");
         return -1;
      }
      read += num;
   }
   
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }

   printf("[test] test passed.\n");
   return 0;
}
