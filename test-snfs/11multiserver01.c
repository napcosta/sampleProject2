/* 
 * Multi-Threaded Server Tests
 * 
 * test-mtswseq.c
 *
 * Tests the multi-threaded server:
 * - tests if the sequencial service of several requests is robust at the
 *   server side. The requests consist on rewriting and checking the same
 *   file several times.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myfs.h>
#include "tfuncs.h"

#define F_NAME "/t.txt"

#define ITERATIONS 20


int main(int argc, char **argv)
{
   // mandatory to init the myfs layer
   my_init_lib();

   // create a file with random data
   char data[4*1024];
   gen_data_rand_zbin(data,sizeof(data));
   tfunc_create_file(F_NAME, data, sizeof(data));
  
   for (int i = 0; i < ITERATIONS; i++) {
      printf("[test] rewriting %d time in %d.\n", i, ITERATIONS);

      // rewrite file with random data
      gen_data_rand_zbin(data, sizeof(data));
      tfunc_rewrite_file(F_NAME, data, sizeof(data));

      // check file integrity
      tfunc_read_and_check_file(F_NAME, data, sizeof(data));
   }

   printf("[test] test passed.\n");
   return 0;
}
