/* 
 * My File System Interface
 * 
 * test-myrw.c
 *
 * Tests the my file system interface:
 * - my_open: open a file
 * - my_close: close a file
 * - my_write: write to the file
 * - my_read: read from file
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the my file system interface
#include <myfs.h>



int main(int argc, char **argv)
{
   int fd, num;

   // mandatory to init the myfs layer
   my_init_lib();

   // create the empty file 'test.txt' in the root directory
   fd = my_open("/test2.txt",O_CREATE);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }

   // fill in a buffer with ascii chars
   char data[2*1024];
   for (int i = 0; i < sizeof(data); data[i]='a'+i%10, i++);

   // write the data to the file
   num = my_write(fd, data, sizeof(data));
   if (num < sizeof(data)) {
      printf("[test] error writing data to file.\n");
      return -1;
   }
   
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }


   /*
    * check that file 'test2' is not corrupt
    */
 
   
   // reopen file 'test2.txt' of the root
   fd = my_open("/test2.txt",0);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }

   // read and compare file contents with written data
   char buffer[512];
   int read = 0;
   while (1) {
      num = my_read(fd,buffer,sizeof(buffer));
      if (num < 0) {
         printf("[test] error reading data from file.\n");
         return -1;
      }
      printf("DEBUG: received %d bytes from read.\n", num);
      if (num == 0) {
         break;
      }
      if (strncmp(&data[read],buffer,num) != 0) {
         printf("[test] file contents differ from data.\n");
         return -1;
      }
      read += num;
   }
   
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      return -1;
   }

   printf("[test] test passed.\n");
   return 0;
}
