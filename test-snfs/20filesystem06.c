/* 
 * File System Tests
 * 
 * test-fsevenodd
 *
 * Tests the file system enhancements:
 * 
 * Test creates 2 clients.
 * Each client tries to create new files.
 * Client A creates files when the number of files is odd;
 * Client B creates files when the number of files is even;
 * Each does this MAX_ITERATIONS times
 * All files are put in the same directory
 */


#include <sys/wait.h>
#include <tfuncs.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <myfs.h>
#include <string.h>
#include "tfuncs.h"

#define nop(t) (sleep(t))


#define BASE_DIR "/dir"

#define MAX_ITERATIONS 10

int parity_client(int parity)
{
   char *fileNames;
   int i=0,myFile=-1;;
   char* parstr = ((parity)?"odd":"even");
   int numberOfFiles;
   char filename[64];

   printf("[%s] started.\n",parstr);
  
   if (my_init_lib() < 0) {
      printf("[test] failed: error initializing the myfs lib.\n");
      exit(-1);
   }

   do {
      if(my_listdir(BASE_DIR, &fileNames, &numberOfFiles)<0){
         printf("[test] failed: error listing directory");
         exit(-1);
      }
      free(fileNames);
      
      if (numberOfFiles%2 == parity) {

         snprintf(filename,sizeof(filename),"%s/%s-%d", BASE_DIR,
            parstr, numberOfFiles);
         printf("[%s] new filename:%s \n", parstr, filename);

         if((myFile = my_open(filename, O_CREATE))<0){
	    printf("[test] failed: error creating file!");
	    exit(-1);
         }
         
         my_close(myFile);
         i++;
      } else {
         printf("[%s] no new files (%d)\n",parstr,numberOfFiles);
      }
      nop(1+parity);
   } while(i < MAX_ITERATIONS);
  
   printf("[%s] ended.\n",parstr);
   return 0;
}

#define EVEN 0
#define ODD 1


int main(int argc, char **argv)
{
  char *filenames;
  int numberOfFilesBefore=-1,numberOfFilesAfter=-1;

  intercept_signals();
  
  my_init_lib();

  if(my_mkdir(BASE_DIR) < 0) {
     printf("[test] failed: unable to create dir.\n");
     return -1;
  }

  if(my_listdir(BASE_DIR, &filenames, &numberOfFilesBefore)<0){
     printf("[test] failed: error listing directory");
     return -1;
  }
  free(filenames);

  if (fork() == 0) {
     exit(parity_client(EVEN));
  }
  
  if (fork() == 0) {
    exit(parity_client(ODD));
  }
  
  int failed = 0;
  int status;
  wait(&status);
  if (WEXITSTATUS(status) != 0) {
    failed = 1;
  }
  wait(&status);
  if (WEXITSTATUS(status) != 0) {
    failed = 1;
  }
  
  if (failed) {
    printf("[test] failed.\n");
  } else {
     
    // check if all files were well created.
    if(my_listdir(BASE_DIR, &filenames, &numberOfFilesAfter)<0){
      printf("[test] failed: error listing directory");
      return -1;
    }
    if ((numberOfFilesAfter-numberOfFilesBefore) != 20){
      printf("Error! %d created files\n", (numberOfFilesAfter-numberOfFilesBefore));
      printf("[test] failed.\n");
      return -1;
    }
    while(numberOfFilesAfter>0){
      printf("%s\n",filenames);
      filenames+=strlen(filenames)+1;
      numberOfFilesAfter--;
    }

    printf("[test] passed.\n");
  }
  return failed;
}
