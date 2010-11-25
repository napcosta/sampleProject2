/* 
 * File System Tests
 * 
 * test-dirbasic
 *
 * Tests the file system enhancements:
 * - creates a set of directories in the root and tests that all were created
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <myfs.h>
#include "tfuncs.h"



#define NUM_DIRS 10


int main(int argc, char **argv)
{
   char dname[1024];

   my_init_lib();


   // create NUM_DIRS directories in the root
   
   for (int i = 0; i < NUM_DIRS; i++) {
      snprintf(dname,sizeof(dname),"/d%d",i);
      if (my_mkdir(dname) < 0) {
         printf("[test] failed: unable to create directory '%s'.\n",dname);
         return -1;
      }
   }


   // check that the directories were created
   
   char* buf;
   int numberOfFiles;
   
   if(my_listdir("/",&buf, &numberOfFiles)<0){
      printf("[test] failed: error listing directory.\n");
      return -1;
   }
   
   if (numberOfFiles != NUM_DIRS) {
      printf("[test] failed: wrong number of files in directory.\n");
      return -1;
   }
   
   for (int i = 0; i < NUM_DIRS; i++) {
      snprintf(dname,sizeof(dname),"d%d",i);
      if(tfunc_dir_search_file(dname,buf,numberOfFiles) <= 0) {
         printf("[test] failed: directory '%s' was not created.\n",dname);
         return -1;
      }
   } 

   printf("Contents of '/':\n");
   tfunc_dir_print(buf,numberOfFiles);

   free(buf);

   printf("[test] passed.\n");
   return 0;
}
