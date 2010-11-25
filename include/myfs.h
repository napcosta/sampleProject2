/* 
 * File System Interface
 * 
 * myfs.h
 *
 * SNFS programming interface simulating the standard Unix I/O interface.
 * 
 */

#ifndef _MYFS_H_
#define _MYFS_H_

struct _file_desc {
	int fileId;
	unsigned size;
	int read_offset;
	int write_offset;
};
typedef struct _file_desc* fd_t;


/*
 * open flags:
 * - O_CERATE: create the file if it does not exist
 */
enum my_open_flags { O_CREATE = 1 };


/*
 * my_init_lib: internal initialization
 */
int my_init_lib();


/*
 * my_open: open the file 'nome'
 * - nome: absolute path of the file to open (e.g. "/f1" refers to
 *   file 'f1' in the root directory)
 * - flags: open flags
 *   returns: the internal file id of the opened file of -1 if error
 */
int my_open(char* nome, int flags);


/*
 * my_read: read data from file
 * - fileId: the file id of the opened file
 * - buffer: where to put the read data [out]
 * - numBytes: maximum number of bytes to read
 *   returns: the number of bytes read, 0 if end of file reached or -1 if error
 */
int my_read(int fileId, char* buffer, unsigned numBytes);


/*
 * my_write: write data to file
 * - fileId: the file id of the opened file
 * - buffer: buffer with the data to write
 * - numBytes: number of bytes to write
 *   returns: the number of bytes written or -1 if error
 */
int my_write(int fileId, char* buffer, unsigned numBytes);


/*
 * my_close: close a previously opened file
 * - fileId: the file id of the file to close
 *   returns: 0 if successful or -1 if error
 */
int my_close(int fileId);


/*
 * my_listdir: list the content of directory 'path'
 * - path: the absolute directory path (e.g. "/", "/d1", "/d1/sd1")
 * - filenames: memory were to write the filenames, each separated 
 *   by '\0', the memory must be freed by the caller [out]
 * - numFiles: number of files written into *filenames [out]
 *   returns: 0 if successful, -1 if error
 */
int my_listdir(char* path, char **filenames, int* numFiles);

int my_mkdir(char* dirname);


#endif 
