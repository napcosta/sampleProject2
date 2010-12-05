/* 
 * My File System Interface
 * 
 * test-append1
 *
 * Tests my file system interface:
 * - creates 2 files and writes data (file sizes are block multiples)
 * - appends one file to another  
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <myfs.h>
#include "tfuncs.h"
#define N_FILES 2
#define F_SIZE (2*512)
#define CHUNK 256

int main(int argc, char **argv)
{
   int num, fd;
   char fdata[N_FILES][F_SIZE];
   char fname[N_FILES][16];
   int ffd[N_FILES];
   char buffer[F_SIZE];
      
   // mandatory to init the myfs layer
   my_init_lib();
   
   // generate files names and random ascii data
   for (int f = 0; f < N_FILES; f++) {
     if (f) {
		  gen_data_rand_ascii(fdata[f],F_SIZE);
		  fdata[f][F_SIZE-1] = '\0';
	  }	  
      sprintf(fname[f],"/f-%d",f);
   }

   /*
    * create and write simultaneously data to files
    */

   // create and open all files
   for (int f = 0; f < N_FILES; f++) {
      ffd[f] = my_open(fname[f],O_CREATE);
      if (ffd[f] < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }
   }

   // write the random data to all files in chunks of 256 bytes 

   for (int ptr = 0; ptr < F_SIZE; ptr += CHUNK) {
      for (int f = 0; f < N_FILES; f++) {
         num = my_write(ffd[f], &fdata[f][ptr], CHUNK);
         if (num < CHUNK) {
            printf("[test] error writing data to file.\n");
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

   // append  file 'f-1' to 'f-0' in the root directory
   fd = my_append("/f-0", "/f-1");
   if (fd < 0) {
      printf("[test] unable to append a file.\n");
      return -1;
   }
   
   // reopen file 'f-0'
   fd = my_open("/f-0",0);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }
    
   // read file contents to buffer
   int ptr = 0;
   num = 0;
   do {
      num = my_read(fd,&buffer[ptr],CHUNK);
      if (num < 0) {
         printf("[test] error reading from file.\n");
         return -1;
      }
      ptr+=num;
   } while(num > 0);
   
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }
   
   // test if file sizes differ
   if (ptr != (2*F_SIZE)) {
      printf("[test] file size differ from the expected value. \n");
      return -1;
   }
   
   printf("[test] PASSED.\n");
   
   exit(0);

}


