/* 
 * My File System Interface
 * 
 * test-mymulr.c
 *
 * Tests the my file system interface:
 * - creates and writes files and checks the
 *   integrity of the written data simultaneously
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myfs.h>
#include "tfuncs.h"


#define N_FILES 4
#define F_SIZE (2*1024)
#define CHUNK 256


int main(int argc, char **argv)
{
   int num;
   
   // mandatory to init the myfs layer
   my_init_lib();

   char fdata[N_FILES][F_SIZE];
   char fname[N_FILES][16];
   int ffd[N_FILES];
   
   // generate files names and random ascii data
   for (int f = 0; f < N_FILES; f++) {
      gen_data_rand_ascii(fdata[f],F_SIZE);
      fdata[f][F_SIZE-1] = '\0';
      sprintf(fname[f],"/f-%d",f);
   }


   /*
    * create and write data to files sequentially
    */

   for (int f = 0; f < N_FILES; f++) {
      
      // create and open the file
      int fd = my_open(fname[f],O_CREATE);
      if (fd < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }

      // write the random data in chunks of 256 bytes
      for (int ptr = 0; ptr < F_SIZE; ptr += CHUNK) {
         num = my_write(fd, &fdata[f][ptr], CHUNK);
         if (num < CHUNK) {
            printf("[test] error writing data to file.\n");
            return -1;
         }
      }

      // close the file
      if (my_close(fd) < 0) {
         printf("[test] error closing file.\n");
         return -1;
      }
   }


   /*
    * check simultaneously that files are not corrupted
    */


   // reopen all files
   for (int f = 0; f < N_FILES; f++) {
      ffd[f] = my_open(fname[f],0);
      if (ffd[f] < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }
   }

   // test the read data in chunks of 256 bytes
   char buffer[CHUNK];
   for (int ptr = 0; ptr < F_SIZE; ptr += CHUNK) {
      for (int f = 0; f < N_FILES; f++) {
         num = my_read(ffd[f], buffer, CHUNK);
         if (num < CHUNK) {
            printf("[test] error reading data from file.\n");
            return -1;
         }
         if (memcmp(&fdata[f][ptr],buffer,num) != 0) {
            printf("[test] file contents differ from data.\n");
            return -1;
         }
       }
   }

   // close all files
   for (int f = 0; f < N_FILES; f++) {
      if (my_close(ffd[f]) < 0) {
         printf("[test] error closing file.\n");
         return -1;
      }
   }

   printf("[test] test passed.\n");
   return 0;
}
