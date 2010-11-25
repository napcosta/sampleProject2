/* 
 * File System Tests
 * 
 * test-dirfiles.c
 *
 * Tests the file system upgrade:
 * - creates files in a directory tree.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myfs.h>
#include "tfuncs.h"



int main(int argc, char **argv)
{
   my_init_lib();


   // create directory tree
   char* dirtree[] = {
      "/a",
      "/a/a",
      "/a/b",
      "/a/c",
      "/a/c/a",
      "/b"
   };

   for (int i = 0; i < sizeof(dirtree)/sizeof(char*); i++) {
      printf("[test] creating dir '%s'\n", dirtree[i]);
      tfunc_dir_make(dirtree[i]);
   }


   // create/open files in the directory tree
   
   char* files[] = {
      "/f1.txt",       // one file in the root
      "/a/f1.txt",     // different files in same directory
      "/a/f2.txt",
      "/a/a/k1.txt",   // open same file
      "/a/a/k1.txt",
      "/a/c/a/x1",     // two files without extension
      "/a/c/a/x2"
   };

   for (int i = 0; i < sizeof(files)/sizeof(char*); i++) {
      printf("[test] create and check file '%s'\n", files[i]);
      tfunc_gen_and_check_file_zbin(files[i], 2*1024);
   }

   printf("[test] test passed.\n");
   return 0;
}
