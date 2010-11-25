/* 
 * File System Tests
 * 
 * test-direrror
 *
 * Tests the file system enhancements:
 * - tests several error conditions w.r.t. directory management
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


   // create a simple file
   tfunc_gen_and_check_file_zbin("/file1.txt", 1025);

   // create a simple directory
   tfunc_dir_make("/dir1");


   /*
    * Test errors creating files
    */

   // condition 1: try to create a file in a non existent directory
   if (my_open("/dir2/file2.txt",O_CREATE) >= 0) {
      printf("[test] open failed condition 1.\n");
      exit(-1);
   }

   // condition 2: use the name of a file as subdirectory name in a path
   if (my_open("/file1.txt/file2.txt",O_CREATE) >= 0) {
      printf("[test] open failed condition 2.\n");
      exit(-1);
   }

   // condition 3: use a path without the root
   if (my_open("file3.txt",O_CREATE) >= 0) {
      printf("[test] open failed: condition 3.\n");
      exit(-1);
   }

   // condition 4: use the name of wrong double slash directory
   if (my_open("//file2.txt",O_CREATE) >= 0) {
      printf("[test] open failed: condition 4.\n");
      exit(-1);
   }


   /*
    * Test errors creating directories
    */

   // condition 1: try to create a directory in a non existent directory
   if (my_mkdir("/dir2/dir2") != -1) {
      printf("[test] mkdir failed: condition 1.\n");
      exit(-1);
   }

   // condition 2: try to create a directory already existent
   if (my_mkdir("/dir1") != -1) {
      printf("[test] mkdir failed: condition 2.\n");
      exit(-1);
   }

   // condition 3: use the name of an existent file
   if (my_mkdir("/file1.txt") != -1) {
      printf("[test] mkdir failed: condition 3.\n");
      exit(-1);
   }

   // condition 4: use the name of a file as subdirectory name in a path
   if (my_mkdir("/file1.txt/dir2") != -1) {
      printf("[test] mkdir failed: condition 4.\n");
      exit(-1);
   }

   // condition 5: create a double shashed directory with subdirs
   if (my_mkdir("//dir2") != -1) {
      printf("[test] mkdir failed: condition 5.\n");
      exit(-1);
   }

   // condition 6: create directory without the root slash
   if (my_mkdir("dir2") != -1) {
      printf("[test] mkdir failed: condition 6.\n");
      exit(-1);
   }


   /*
    * Check that after all this mess '/' contains only 'file1.txt' and 'dir1'
    */
   
   char* buf;
   int numberOfFiles;
   
   if(my_listdir("/",&buf, &numberOfFiles)<0){
      printf("[test] failed: error listing directory.\n");
      return -1;
   }

   printf("Contents of '/':\n");
   tfunc_dir_print(buf,numberOfFiles);

   if (numberOfFiles != 2) {
      printf("[test] failed: wrong number of files in directory.\n");
      return -1;
   }
   
   if(tfunc_dir_search_file("file1.txt",buf,numberOfFiles) <= 0) {
       printf("[test] failed: '/file1.txt' not found.\n");
       return -1;
   } 

   if(tfunc_dir_search_file("dir1",buf,numberOfFiles) <= 0) {
       printf("[test] failed: '/dir1' not found.\n");
       return -1;
   }

   free(buf);

   printf("[test] passed.\n");
   return 0;
}

