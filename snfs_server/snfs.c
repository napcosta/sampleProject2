/* 
 * SNFS Logic Layer
 * 
 * snfs.c
 *
 * Implementation of the SNFS server logic and service handlers. 
 * The handlers also have to format data from and to the standard
 * message formats associated with requests and responses.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <snfs_proto.h>
#include "block.h"
#include "fs.h"


#ifndef NUM_BLOCKS
// default storage of 8 MB (8*1024*2 blocks * 512 bytes/block)
#define NUM_BLOCKS (8*1024*2)
#endif

#define DEFAULT_DISK_DELAY 10000

static fs_t* FS;


void snfs_init(int argc, char **argv)
{
  int disk_delay = DEFAULT_DISK_DELAY;
  if (argc > 1)
    sscanf(argv[1], "%d", &disk_delay);
  FS = fs_new(NUM_BLOCKS, disk_delay);
  fs_format(FS);
}


void snfs_ping(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   printf("[snfs] handling a 'ping' request.\n");

   // retrieve the content of the message
   printf("[snfs] received msg='%s'.\n",req->body.ping.msg);

   // format the response to the client
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.ping);
   res->type = REQ_PING;
   res->status = RES_OK;
   

   /*
    * complete the response to the client sending it a hello 
    * string; fill in the field res->body.ping.msg 
    * (see the corresponding type in file include/snfs_proto.h to understand 
    * the structure of messages)
    */
}


void snfs_lookup(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   // get input arguments
   char* file = req->body.lookup.pname;

   // prepare response
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.lookup);
   res->type = REQ_LOOKUP;
   res->status = RES_ERROR;
  
   // handle request

   inodeid_t fileid;
   if (fs_lookup(FS,file,&fileid) == 1) {
      fs_file_attrs_t attrs;
      if (fs_get_attrs(FS,fileid,&attrs) == 0) {
         res->status = RES_OK;
         res->body.lookup.file = fileid;
         res->body.lookup.fsize = attrs.size;
      }
   }   
}


void snfs_read(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   // get input arguments
   inodeid_t file = (inodeid_t)req->body.read.fhandle;
   unsigned offset = req->body.read.offset;
   unsigned count = req->body.read.count;
   
   // prepare the response
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.read);
   res->type = REQ_READ;
   res->status = RES_ERROR;

   // handle request
   char* data = res->body.read.data;
   int* nread = (int*)&res->body.read.nread;
   if (count <= MAX_READ_DATA && !fs_read(FS,file,offset,count,data,nread)){
      res->status = RES_OK;
   }
}


void snfs_write(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   // get input arguments
   inodeid_t file = (inodeid_t)req->body.write.fhandle;
   unsigned offset = req->body.write.offset;
   unsigned count = req->body.write.count;
   char* data = req->body.write.data;

   // prepare the response
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.write);
   res->type = REQ_WRITE;
   res->status = RES_ERROR;

   // handle request
   if (count <= MAX_WRITE_DATA && !fs_write(FS,file,offset,count,data)){
      fs_file_attrs_t attrs;
      if (fs_get_attrs(FS,file,&attrs) == 0) {
         res->status = RES_OK;
         res->body.write.fsize = attrs.size;
      }
   }
}


void snfs_create(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   // get input arguments
   inodeid_t dir = (inodeid_t)req->body.create.dir;
   char* name = req->body.create.name;

   // format the response
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.create);
   res->type = REQ_CREATE;
   res->status = RES_ERROR;

   // handle request
   name[MAX_FILE_NAME_SIZE-1] = '\0';
   inodeid_t fileid;
   if (!fs_create(FS,dir,name,&fileid)) {
      res->status = RES_OK;
      res->body.create.file = (snfs_fhandle_t)fileid;
   }
}


void snfs_mkdir(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   // get input arguments
   inodeid_t dir = (inodeid_t)req->body.mkdir.dir;
   char* f = req->body.mkdir.file;

   // format the response
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.mkdir);
   res->type = REQ_MKDIR;
   res->status = RES_ERROR;

   // handle request
   f[MAX_FILE_NAME_SIZE-1] = '\0';
   inodeid_t dirid;
   if (fs_mkdir(FS,dir,f,&dirid)==0) {
      res->status = RES_OK;
      res->body.mkdir.newdirid = (snfs_fhandle_t) dirid;
   }
}


void snfs_readdir(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, 
   int* ressz)
{
   // get input arguments
   inodeid_t dir = (inodeid_t)req->body.readdir.dir;
   unsigned maxentries = req->body.readdir.cmax;
   
   // format the response
   *ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.readdir);
   res->type = REQ_READDIR;
   res->status = RES_ERROR;

   // handle request
   fs_file_name_t entries[MAX_READDIR_ENTRIES];
   int numentries;
   if (!fs_readdir(FS,dir,entries,maxentries,&numentries)) {
      res->status = RES_OK;
      res->body.readdir.count = numentries;
      for (int i = 0; i < numentries; i++) {
         snfs_dir_entry_t* entry = &res->body.readdir.list[i]; 
         strncpy(entry->name,entries[i].name,MAX_FILE_NAME_SIZE);
         entry->len = strlen(entry->name);
         switch (entries[i].type) {
            case FS_DIR:
               entry->type = SNFS_DIR;
               break;
            case FS_FILE:
               entry->type = SNFS_FILE;
               break;
            default:
               printf("[snfs] severe error unknown file type.\n");
               exit(-1);
         }
      }
   }
}


void snfs_remove(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, int* ressz)
{
	printf("[snfs] handling a 'remove' request");
	char name[50];
	// get input arguments
	inodeid_t dir = (inodeid_t)req->body.remove.dir;
	strcpy(name,  req->body.remove.name);
	
	// format the response to the client
	*ressz = sizeof(*res) - sizeof(res->body) + sizeof(res->body.remove);
	res->type = REQ_REMOVE;
	res->status = RES_ERROR;
	
	if (fs_remove(FS, dir, name) == 0) {
		res->status = RES_OK;
	}


}
		   
void snfs_copy(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, int* ressz)
{

// IMPLEMENT

}	   

void snfs_append(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, int* ressz)
{

// IMPLEMENT

}	   
		   
void snfs_defrag(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, int* ressz)
{

// IMPLEMENT

}	   
		   
void snfs_diskusage(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, int* ressz)
{

// IMPLEMENT

}	   
		   
void snfs_dumpcache(snfs_msg_req_t *req, int reqsz, snfs_msg_res_t *res, int* ressz)
{

// IMPLEMENT

}	   
		   
