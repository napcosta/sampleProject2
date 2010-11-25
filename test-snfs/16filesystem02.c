/* 
 * File System Tests
 * 
 * test-dirtree
 *
 * Tests the file system upgrade:
 * - creates a small tree of subdirectories in the filesystem
 * 
 * The following directory tree is created
 * 
 *  /b1
 *  /b1/a1
 *  /b1/a2
 *  /b2
 *  /b2/a1
 *  /b2/a2
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


   // print directory tree info
   
   int degree = 2;
   int depth = 2;
   int dirs = 7;
   
   printf("Creating the following directory tree with:\n");
   printf("- %d subdirectories per directory\n", degree);
   printf("- %d subdirectory depth\n", depth);
   printf("- %d total directories\n\n", dirs);
   
   tfunc_tree_print(degree, depth, dirs);


   // create the directory tree

   tfunc_tree_mkdir(degree, depth, dirs);


   // check integrity of the directory tree

   tfunc_tree_inspect(degree, depth, dirs);


   printf("[test] passed.\n");
   return 0;
}
