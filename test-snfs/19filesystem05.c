/* 
 * File System Tests
 * 
 * test-dirmaxtree
 *
 * Tests the file system upgrade:
 * - fills the file system with a tree of directories
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


   // the max number of directories to create is 62
   //
   // max num i-nodes (64) - unused inode #0 - root inode #1
   
   
   // print directory tree info
   
   int degree = 3;
   int depth = 4;
   int dirs = 62;

   printf("Creating the following directory tree with:\n");
   printf("- %d subdirectories per directory\n", degree);
   printf("- %d subdirectory depth\n", depth);
   printf("- %d total directories\n\n", dirs);
   
   tfunc_tree_print(degree, depth, dirs);


   // create the directory tree

   tfunc_tree_mkdir(degree, depth, dirs);


   // check integrity of the directory tree

   tfunc_tree_inspect(degree, depth, dirs);


   // try to create one last directory - it must fail
   if (my_mkdir("/z") != -1) {
      printf("[test] failed: max number of i-nodes reached.\n");
      exit(-1);
   }

   printf("[test] passed.\n");
   return 0;
}
