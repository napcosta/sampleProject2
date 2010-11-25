/* 
 * Auxiliary test functions
 */

#ifndef _TFUNCS_H_
#define _TFUNCS_H_

void tfunc_create_file(char* file, char* data, int size);
   
void tfunc_rewrite_file(char* file, char* data, int size);

void tfunc_read_and_check_file(char* file, char* tocompare, int size);

int tfunc_dir_search_file(char* file, char* buf, int numberOfFiles);

void tfunc_dir_print(char* buf, int numberOfFiles);

int tfunc_dir_make(char* path);

void tfunc_gen_and_check_file_zbin(char* file, int size);

void tfunc_tree_print(int degree, int max_depth, int max_dirs);

void tfunc_tree_mkdir(int degree, int max_depth, int max_dirs);

void tfunc_tree_inspect(int degree, int max_depth, int max_dirs);

void msg_test_passed(char* msg);

void msg_test_failed(char* msg);

void msg_test_failed_motive(char* msg);

void gen_data_rand_ascii(char* block, int size);

void gen_data_rand_str(char* block, int size);

void gen_data_rand_bin(char* block, int size);

void gen_data_rand_zbin(char* block, int size);

void gen_data_incr_bin(char* block, int size);

int compare_data(char* b1, char* b2, int size);

void intercept_signals();

#endif
