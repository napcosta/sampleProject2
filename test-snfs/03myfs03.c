/* 
 * My File System Interface
 * 
 * test-mylistdir
 *
 * Tests the my file system interface:
 * - open: open a file
 * - close: close a file
 * - listdir: get the contents of directory
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the my file system interface
#include <myfs.h>


int search_file(char* file) {
   char* buf,*current;
   int numberOfFiles,j;
  
   if(my_listdir("/",&buf, &numberOfFiles)<0){
      printf("[test] error listing directory.\n");
      return -1;
   }
   current = buf;
   for(j=0;j<numberOfFiles;j++){
      if (!strcmp(file,current)) {
         return 1;
      }
      current+=strlen(current)+1;
   }
   free(buf);
   return 0;
}


#define NUM_FILES 5


int main(int argc, char **argv)
{
   int fd;
   char fname[1024];

   my_init_lib();

   for (int i = 0; i < NUM_FILES; i++) {
      snprintf(fname,sizeof(fname),"/f-%d",i);

      // create an empty file
      fd = my_open(fname,O_CREATE);
      if (fd < 0) {
         printf("[test] unable to open file.\n");
         return -1;
      }

      // close the file
      if (my_close(fd) < 0) {
         printf("[test] error closing file.\n");
         return -1;
      }

      // check that the file was added to the directory
      if(search_file(&fname[1]) <= 0) { // remove the '/'
         printf("[test] file was not added to the directory.\n");
         exit(-1);
      }
   }

   printf("[test] passed.\n");
   return 0;
}
