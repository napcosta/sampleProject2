/* 
 * My File System Interface
 * 
 * test-mydefrag1
 *
 * Tests my file system interface:
 * - creates and writes data to 2 files
 * - removes the first file
 * - defragments the file system 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <myfs.h>
#include "tfuncs.h"
#define N_FILES 2
#define F_SIZE 2*512
#define CHUNK 256


int main(int argc, char **argv)
{

   int fd, num;
   char fdata[N_FILES][F_SIZE];
   char fname[N_FILES][16];
   int ffd[N_FILES];

   // mandatory to init the myfs layer
   my_init_lib();
   // generate files names and random ascii data
   for (int f = 0; f < N_FILES; f++) {
      gen_data_rand_ascii(fdata[f],F_SIZE);
      fdata[f][F_SIZE-1] = '\0';
      sprintf(fname[f],"/f-%d",f);
   }

   /*
    * create and write data to the files
    */

   // create and open the file
   for (int f = 0; f < N_FILES; f++) {
      ffd[f] = my_open(fname[f],O_CREATE);
      if (ffd[f] < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }
   }

   // write the random data to the files in chunks of 256 bytes
   for (int ptr = 0; ptr < F_SIZE; ptr += CHUNK) {
      for (int f = 0; f < N_FILES; f++) {
         num = my_write(ffd[f], &fdata[f][ptr], CHUNK);
         if (num < CHUNK) {
            printf("[test] error writing data to file.\n");
            return -1;
         }
       }
   }

   // close the files
   for (int f = 0; f < N_FILES; f++) {
      if (my_close(ffd[f]) < 0) {
         printf("[test] error closing file.\n");
         return -1;
      }
   }

   // removes  file 'f-0' from the root directory
   fd = my_remove("/f-0");
   if (fd < 0) {
      printf("[test] unable to remove file.\n");
      return -1;
   }

   // Dump file system occupied data blocks 
   printf("\n---> disk dumping before defragmentation <----\n");
   if(my_diskusage( )<0){
      printf("[test] error dumping occupied data blocks.\n");
      return -1;
   }

   // defragment
   fd = my_defrag();
   if (fd < 0) {
      printf("[test] unable to defragment the FS.\n");
      return -1;
   }
   
    // Dump file system occupied data blocks 
   printf("\n---> disk dumping after defragmentation <----\n"); 	
   if(my_diskusage( )<0){
      printf("[test] error dumping occupied data blocks.\n");
      return -1;
   }
  
      printf("\n[test] defragment was successfull if:\n	- before defragmentation: blocks 12 and 14 are allocated to file '/f-1'.\n	- after defragmentation: the 2 blocks allocated to file '/f-1' are contiguous.\n\n	(Please check diskusage dumping on the server terminal).\n\n");
 
   return 0;
}


