/* Auxiliary test functions
 */

#define __USE_POSIX

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <myfs.h>

#define Fout stdout


void gen_data_rand_ascii(char* block, int size)
{
   int value = 0;
   /* fill the block with alfanum data */
   for (int i = 0; i < size; i++) {
      value = rand()%122;
      if (48 > value)
         value += 48;
      if ((57 < value) && (65 > value))
         value += 7;
      if (( 90 < value ) && ( 97 > value))
         value += 6;
      block[i] = (char)value;
   }
}


void gen_data_rand_str(char* block, int size)
{
   int value = 0;
   /* fill the block with alfanum data, null terminated */
   for (int i = 0; i < size; i++) {
      value = rand()%122;
      if (48 > value)
         value += 48;
      if ((57 < value) && (65 > value))
         value += 7;
      if (( 90 < value ) && ( 97 > value))
         value += 6;
      block[i] = (char)value;
   }
   block[size-1] = '\n';
}


void gen_data_rand_bin(char* block, int size)
{
   /* fill the block with binary data */
   for (int i = 0; i < size; i++) {
      block[i] = (char)rand();
   }
} 


void gen_data_rand_zbin(char* block, int size)
{
   /* fill the block with binary data and zeros in a 10 byte frequency */
   for (int i = 0; i < size; i++) {
      block[i] = ((i+1)%10==0)?('\0'):((char)rand());
   }
} 


void gen_data_incr_bin(char* block, int size)
{
   /* fill the block with binary data, growing from 0 */
   for (int i = 0; i < size; i++) {
      block[i] = (char)i;
   }
}


void tfunc_create_file(char* file, char* data, int size)
{
   // create a file
   int fd = my_open(file,O_CREATE);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      exit(-1);
   }

   // write the data to the file
   int num = my_write(fd, data, size);
   if (num < size) {
      printf("[test] error writing data to file.\n");
      exit(-1);
   }

   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      exit(-1);
   }
}


void tfunc_rewrite_file(char* file, char* data, int size)
{
   // open the file
   int fd = my_open(file,0);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      exit(-1);
   }

   // write the data to the file
   int num = 0;
   for (int written = 0; written < size; written += num) {
      int left = size - written;
      int towrite = ((left > 117)?117:left);
      num = my_write(fd, data + written, towrite);
      if (num != towrite) {
         printf("[test] error writing data to file.\n");
         exit(-1);
      }
   }

   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      exit(-1);
   }
}


void tfunc_read_and_check_file(char* file, char* tocompare, int size)
{
   // reopen file
   int fd = my_open(file,0);
   if (fd < 0) {
      printf("[test] unable to open file.\n");
      exit(-1);
   }

   // read and compare file contents with written data
   char buffer[512];
   int num, read = 0;
   while (1) {
      num = my_read(fd,buffer,sizeof(buffer));
      if (num < 0) {
         printf("[test] error reading data from file.\n");
         exit(-1);
      }
      if (num == 0 && read == size) {
         break;
      }
      if (num == 0 || read + num > size) {
         printf("[test] file size differ from expected.\n");
         exit(-1);
      }
      if (memcmp(&tocompare[read],buffer,num) != 0) {
         printf("[test] file contents differ from data.\n");
         exit(-1);
      }
      read += num;
   }
   
   // close the file
   if (my_close(fd) < 0) {
      printf("[test] error closing file.\n");
      exit(-1);
   }
}


int tfunc_dir_search_file(char* file, char* buf, int numberOfFiles) {
   char *current;
   int j;
  
   current = buf;
   for(j=0;j<numberOfFiles;j++){
      if (!strcmp(file,current)) {
         return 1;
      }
      current+=strlen(current)+1;
   }
   return 0;
}


void tfunc_dir_print(char* buf, int numberOfFiles) {
   char *current;
   int j;
  
   current = buf;
   for(j=0;j<numberOfFiles;j++){
      printf("%s\n",current);
      current+=strlen(current)+1;
   }
}


void tfunc_gen_and_check_file_zbin(char* file, int size)
{
   // fill in a buffer with binary data with zeros
   char data[size];
   gen_data_rand_zbin(data,sizeof(data));

   // create a file
   tfunc_create_file(file, data, sizeof(data));

   // check file integrity
   tfunc_read_and_check_file(file, data, sizeof(data));
}


/*
 * Directory tree functions
 */


int tfunc_tree_traverse(char* path, int degree, int max_depth, int max_dirs,
   int (*func)(char*))
{
   char dname[1024];

   if (max_depth > 0) {
      for (int i = 1; i <= degree; i++) {
         if (max_dirs == 0) {
            return 0;
         } else {

            // generate the name of the current path
            snprintf(dname,sizeof(dname),"%s/%c%d",path,
               (('a' + max_depth - 1) % 'z'),i);

            // call the function
            if (func(dname) < 0) {
               return -1;
            }

            // traverse the subtree
            int out = tfunc_tree_traverse(dname, degree, max_depth-1, 
               max_dirs-1, func);
            if (out < 0) {
               return -1;
            } else {
               max_dirs = out;
            }
         }
      }
   }
   return max_dirs;
}


int tfunc_tree_inspect_r(char* path, int degree, int max_depth, int max_dirs)
{
   char dname[1024];

   if (max_depth > 0) {

      printf("Inspecting: %s\n", path);

      // get directory list from current path
      char* buf;
      int numberOfFiles;

      if(my_listdir(path, &buf, &numberOfFiles)<0){
         printf("[test] failed: error listing directory.\n");
         exit(-1);
      }

      if (numberOfFiles > degree || numberOfFiles < 0) {
         printf("[test] failed: wrong number of files in directory.\n");
         exit(-1);
      }

      // inspect subdirectories
      for (int i = 1; i <= degree; i++) {
         if (max_dirs == 0) {
            return 0;
         }

         // generate the name of the current path
         snprintf(dname,sizeof(dname),path[1] ? "%s/%c%d" : "%s%c%d",path,
            (('a' + max_depth - 1) % 'z'),i);


         // it must be in the directory list
         if(tfunc_dir_search_file(strrchr(dname, '/')+1,buf,numberOfFiles) <= 0) {
            printf("[test] failed: directory '%s' does not exist.\n",dname);
            exit(-1);
         }

         // check the subtree
         max_dirs = tfunc_tree_inspect_r(dname, degree, max_depth-1,
            max_dirs-1);
      }

      free(buf);
   }
   return max_dirs;
}


static int print_name(char* path)
{
   printf("%s\n", path);
   return 0;
}


int tfunc_dir_make(char* path)
{
   if (my_mkdir(path) < 0) {
      printf("[test] failed: unable to create directory '%s'.\n",path);
      exit(-1);
   }
   return 0;
}


void tfunc_tree_print(int degree, int max_depth, int max_dirs)
{
   tfunc_tree_traverse("", degree, max_depth, max_dirs, print_name);
}


void tfunc_tree_mkdir(int degree, int max_depth, int max_dirs)
{
   tfunc_tree_traverse("", degree, max_depth, max_dirs, tfunc_dir_make);
}


void tfunc_tree_inspect(int degree, int max_depth, int max_dirs)
{
   tfunc_tree_inspect_r("/", degree, max_depth, max_dirs);
}


void msg_test_passed(char* msg)
{
   if (msg == NULL) {
      fprintf(Fout,"\nTest passed.\n");
   } else {
      fprintf(Fout,"\nTest passed: %s\n",msg);
   }
}


void msg_test_failed(char* msg)
{
   if (msg == NULL) {
      fprintf(Fout,"\nTest failed.\n");
   } else {
      fprintf(Fout,"\nTest failed: %s\n",msg);
   }
}


void msg_test_failed_motive(char* msg)
{
   if (msg == NULL) {
      fprintf(Fout,"\nTest failed.\n");
   } else {
      fprintf(Fout,"\nTest failed (possible motive): %s\n",msg);
   }
}


int compare_data(char* b1, char* b2, int size)
{
   for (int i = 0; i < size; i++) {
      if (b1[i] != b2[i]) {
         return 0;
      }
   }
   return 1;
} 


void catch_segmentation_fault(int signal)
{
   msg_test_failed("segmentation fault occurred.");
   exit(0);
}


void catch_broken_pipe(int signal)
{
   msg_test_failed("unhandled broken pipe.");
   exit(-2);
}


void intercept_signals()
{
   signal(SIGSEGV,catch_segmentation_fault);
   signal(SIGPIPE,catch_broken_pipe);
}

