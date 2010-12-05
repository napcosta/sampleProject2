/* 
 * Tests if the two blocks written are in the cache
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <myfs.h>


int main(int argc, char **argv)
{
   int fd, num;

   // mandatory to init the myfs layer
   my_init_lib();

   // create the empty file 'cache1.txt' in the root directory
   fd = my_open("/cache1.txt",O_CREATE);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      return -1;
   }

   // fill in a buffer with ascii chars
   char data[1024];
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
    * check that file 'cache1.txt' is not corrupted
    */
   
   fd = my_open("/cache1.txt",0);
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
    	
   // Dump server's cache of blocks 
/*  if(my_dumpcache()<0){
      printf("[test] error server's cache dumping.\n");
      return -1;
   }
*/      
   printf("\n[test]  Passed, if the cache dumping on the server terminal shows:\n		- blocks 11 and 12 are in the cache.\n		- and the remaining entries are empty or not valid.\n\n	(Please check cache dumping on the server terminal).\n\n");
   
   return 0;
}


