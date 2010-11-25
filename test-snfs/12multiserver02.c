/* 
 * Multi-Threaded Server Tests
 * 
 * test-mtsrseq.c
 *
 * Tests the multi-threaded server:
 * - tests if the sequencial service of several requests is robust at the
 *   server side.
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


   // fill in a buffer with binary data with zeros
   char data[4*1024];
   gen_data_rand_zbin(data,sizeof(data));

   // create a file
   tfunc_create_file(F_NAME, data, sizeof(data));
   
   // check file integrity several times in sequence
   for (int i = 0; i < ITERATIONS; i++) {
      printf("[test] testing %d time in %d.\n", i, ITERATIONS);
      tfunc_read_and_check_file(F_NAME, data, sizeof(data));
   }

   printf("[test] test passed.\n");
   return 0;
}
