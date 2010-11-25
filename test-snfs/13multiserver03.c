/* 
 * Multi-Threaded Server Tests
 * 
 * test-mtswpar.c
 *
 * Tests the multi-threaded server:
 * - tests if the paralel service of several requests is robust at the
 *   server side. Several clients are forked and do paralel sequence of
 *   writes into different files
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <myfs.h>
#include "tfuncs.h"

#define ITERATIONS 10
#define NUM_CLIS 4


int main(int argc, char **argv)
{
   // mandatory to init the myfs layer
   my_init_lib();


   // launch NUM_CLIS clients in parallel
   for (int procs = 0; procs < NUM_CLIS; procs++) {
      int stat = fork();
      if (stat < 0) {
         printf("[test] error forking.\n");
         return -1;
      }
      if (stat == 0) {
         my_init_lib();

         // fill in a buffer with binary data with zeros
         char data[4*1024];
         gen_data_rand_zbin(data,sizeof(data));

         // create a file
         char filename[64];
         sprintf(filename,"/t-%d",getpid());
         tfunc_create_file(filename, data, sizeof(data));

         for (int i = 0; i < ITERATIONS; i++) {
            // rewrite file with random data
            gen_data_rand_zbin(data, sizeof(data));
            tfunc_rewrite_file(filename, data, sizeof(data));

            // check file integrity
            tfunc_read_and_check_file(filename, data, sizeof(data));
         }
         printf("[test] cli %d - passed!\n", getpid());
         exit(0);
      }
      printf("[test] cli %d - created.\n", stat);
   }

   // wait for the clients to finish
   for (int procs = 0; procs < NUM_CLIS; procs++) {
      int pid, status;
      pid = wait(&status);
      printf("[test] cli %d - ended.\n", pid);
   }

   printf("[test] check that all %d clients passed.\n", NUM_CLIS);
   return 0;
}
