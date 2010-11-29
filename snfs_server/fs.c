/* 
 * File System Layer
 * 
 * fs.c
 *
 * Implementation of the file system layer. Manages the internal 
 * organization of files and directories in a 'virtual memory disk'.
 * Implements the interface functions specified in fs.h.
 *
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "fs.h"


#define dprintf if(1) printf

#define BLOCK_SIZE 512

/*
 * Inode
 * - inode size = 64 bytes
 * - num of direct block refs = 10 blocks
 */

#define INODE_NUM_BLKS 10

#define EXT_INODE_NUM_BLKS (BLOCK_SIZE / sizeof(unsigned int))

typedef struct fs_inode {
   fs_itype_t type;
   unsigned int size;
   unsigned int blocks[INODE_NUM_BLKS];
   unsigned int reserved[4]; // reserved[0] -> extending table block number
} fs_inode_t;

typedef unsigned int fs_inode_ext_t;


/*
 * Directory entry
 * - directory entry size = 16 bytes
 * - filename max size - 14 bytes (13 chars + '\0') defined in fs.h
 */

#define DIR_PAGE_ENTRIES (BLOCK_SIZE / sizeof(fs_dentry_t))

typedef struct dentry {
   char name[FS_MAX_FNAME_SZ];
   inodeid_t inodeid;
} fs_dentry_t;


/*
 * File syste structure
 * - inode table size = 64 entries (8 blocks)
 * 
 * Internal organization 
 *   - block 0        - free block bitmap
 *   - block 1        - free inode bitmap
 *   - block 2-9      - inode table (8 blocks)
 *   - block 10-(N-1) - data blocks, where N is the number of blocks
 */

#define ITAB_NUM_BLKS 8

#define ITAB_SIZE (ITAB_NUM_BLKS*BLOCK_SIZE / sizeof(fs_inode_t))

struct fs_ {
   blocks_t* blocks;
   char inode_bmap [BLOCK_SIZE];
   char blk_bmap [BLOCK_SIZE];
   fs_inode_t inode_tab [ITAB_SIZE];
};

#define NOT_FS_INITIALIZER  1
                               
/*
 * Internal functions for loading/storing file system metadata do the blocks
 */
                                
                                
static void fsi_load_fsdata(fs_t* fs)
{
   blocks_t* bks = fs->blocks;
   
   // load free block bitmap from block 0
   block_read(bks,0,fs->blk_bmap);

   // load free inode bitmap from block 1
   block_read(bks,1,fs->inode_bmap);
   
   // load inode table from blocks 2-9
   for (int i = 0; i < ITAB_NUM_BLKS; i++) {
      block_read(bks,i+2,&((char*)fs->inode_tab)[i*BLOCK_SIZE]);
   }
#define NOT_FS_INITIALIZER  1  //file system is already initialized, subsequent block acess will be delayed using a sleep function.
}


static void fsi_store_fsdata(fs_t* fs)
{
   blocks_t* bks = fs->blocks;
 
   // store free block bitmap to block 0
   block_write(bks,0,fs->blk_bmap);

   // store free inode bitmap to block 1
   block_write(bks,1,fs->inode_bmap);
   
   // store inode table to blocks 2-9
   for (int i = 0; i < ITAB_NUM_BLKS; i++) {
      block_write(bks,i+2,&((char*)fs->inode_tab)[i*BLOCK_SIZE]);
   }
}


/*
 * Bitmap management macros and functions
 */

#define BMAP_SET(bmap,num) ((bmap)[(num)/8]|=(0x1<<((num)%8)))

#define BMAP_CLR(bmap,num) ((bmap)[(num)/8]&=~((0x1<<((num)%8))))

#define BMAP_ISSET(bmap,num) ((bmap)[(num)/8]&(0x1<<((num)%8)))


static int fsi_bmap_find_free(char* bmap, int size, unsigned* free)
{
   for (int i = 0; i < size; i++) {
      if (!BMAP_ISSET(bmap,i)) {
         *free = i;
         return 1;
      }
   }
   return 0;
}


static void fsi_dump_bmap(char* bmap, int size)
{
   int i = 0;
   for (; i < size; i++) {
      printf("%x.", (unsigned char)bmap[i]);
      if (i > 0 && i % 32 == 0) {
         printf("\n");
      }
   }
}


/*
 * Other internal file system macros and functions
 */

#define MIN(a,b) ((a)<=(b)?(a):(b))
                                
#define MAX(a,b) ((a)>=(b)?(a):(b))
                                
#define OFFSET_TO_BLOCKS(pos) ((pos)/BLOCK_SIZE+(((pos)%BLOCK_SIZE>0)?1:0))

                                
static void fsi_inode_init(fs_inode_t* inode, fs_itype_t type)
{
   int i;
   
   inode->type = type;
   inode->size = 0;
   for (i = 0; i < INODE_NUM_BLKS; i++) {
      inode->blocks[i] = 0;
   }
   
   for (i = 0; i < 4; i++) {
	   inode->reserved[i] = 0;
   }
}


static int fsi_dir_search(fs_t* fs, inodeid_t dir, char* file, 
   inodeid_t* fileid)
{
   fs_dentry_t page[DIR_PAGE_ENTRIES];
   fs_inode_t* idir = &fs->inode_tab[dir];
   int num = idir->size / sizeof(fs_dentry_t);
   int iblock = 0;

   while (num > 0) {
      block_read(fs->blocks,idir->blocks[iblock++],(char*)page);
      for (int i = 0; i < DIR_PAGE_ENTRIES && num > 0; i++, num--) {
         if (strcmp(page[i].name,file) == 0) {
            *fileid = page[i].inodeid;
            return 0;
         }
      }
   }
   return -1;
}


/*
 * File system interface functions
 */


void io_delay_on(int disk_delay);

fs_t* fs_new(unsigned num_blocks, int disk_delay)
{
   fs_t* fs = (fs_t*) malloc(sizeof(fs_t));
   fs->blocks = block_new(num_blocks,BLOCK_SIZE);
   fsi_load_fsdata(fs);
   io_delay_on(disk_delay);
   return fs;
}


int fs_format(fs_t* fs)
{
   if (fs == NULL) {
      printf("[fs] argument is null.\n");
      return -1;
   }

   // erase all blocks
   char null_block[BLOCK_SIZE];
   memset(null_block,0,sizeof(null_block));
   for (int i = 0; i < block_num_blocks(fs->blocks); i++) {
      block_write(fs->blocks,i,null_block);
   }

   // reserve file system meta data blocks
   BMAP_SET(fs->blk_bmap,0);
   BMAP_SET(fs->blk_bmap,1);
   for (int i = 0; i < ITAB_NUM_BLKS; i++) {
      BMAP_SET(fs->blk_bmap,i+2);
   }

   // reserve inodes 0 (will never be used) and 1 (the root)
   BMAP_SET(fs->inode_bmap,0);
   BMAP_SET(fs->inode_bmap,1);
   fsi_inode_init(&fs->inode_tab[1],FS_DIR);

   // save the file system metadata
   fsi_store_fsdata(fs);
   return 0;
}



int fs_get_attrs(fs_t* fs, inodeid_t file, fs_file_attrs_t* attrs)
{
   if (fs == NULL || file >= ITAB_SIZE || attrs == NULL) {
      dprintf("[fs_get_attrs] malformed arguments.\n");
      return -1;
   }

   if (!BMAP_ISSET(fs->inode_bmap,file)) {
      dprintf("[fs_get_attrs] inode is not being used.\n");
      return -1;
   }

   fs_inode_t* inode = &fs->inode_tab[file];
   attrs->inodeid = file;
   attrs->type = inode->type;
   attrs->size = inode->size;
   switch (inode->type) {
      case FS_DIR:
         attrs->num_entries = inode->size / sizeof(fs_dentry_t);
         break;
      case FS_FILE:
         attrs->num_entries = -1;
         break;
      default:
         dprintf("[fs_get_attrs] fatal error - invalid inode.\n");
         exit(-1);
   }
   return 0;
}


int fs_lookup(fs_t* fs, char* file, inodeid_t* fileid)
{

char *token;
char line[MAX_PATH_NAME_SIZE]; 
char *search = "/";
int i=0;
int dir=0;
   if (fs==NULL || file==NULL ) {
      dprintf("[fs_lookup] malformed arguments.\n");
      return -1;
   }


    if(file[0] != '/') {
        dprintf("[fs_lookup] malformed pathname.\n");
        return -1;
    }

    strcpy(line,file);
    token = strtok(line, search);
    
   while(token != NULL) {
     i++;
     if(i==1) dir=1;  //Root directory
     
     if (!BMAP_ISSET(fs->inode_bmap,dir)) {
	      dprintf("[fs_lookup] inode is not being used.\n");
	      return -1;
     }
     fs_inode_t* idir = &fs->inode_tab[dir];
     if (idir->type != FS_DIR) {
        dprintf("[fs_lookup] inode is not a directory.\n");
        return -1;
     }
     inodeid_t fid;
     if (fsi_dir_search(fs,dir,token,&fid) < 0) {
        dprintf("[fs_lookup] file does not exist.\n");
        return 0;
     }
     *fileid = fid;
     dir=fid;
     token = strtok(NULL, search);
   }

   return 1;
}


int fs_read(fs_t* fs, inodeid_t file, unsigned offset, unsigned count, 
   char* buffer, int* nread)
{
	if (fs==NULL || file >= ITAB_SIZE || buffer==NULL || nread==NULL) {
		dprintf("[fs_read] malformed arguments.\n");
		return -1;
	}

	if (!BMAP_ISSET(fs->inode_bmap,file)) {
		dprintf("[fs_read] inode is not being used.\n");
		return -1;
	}

	fs_inode_t* ifile = &fs->inode_tab[file];
	if (ifile->type != FS_FILE) {
		dprintf("[fs_read] inode is not a file.\n");
		return -1;
	}

	if (offset >= ifile->size) {
		*nread = 0;
		return 0;
	}
	
   	// read the specified range
	int pos = 0;
	int iblock = offset/BLOCK_SIZE;
	int blks_used = OFFSET_TO_BLOCKS(ifile->size);
	int max = MIN(count,ifile->size-offset);
	int tbl_pos;
	unsigned int *blk;
	char block[BLOCK_SIZE];
   
	while (pos < max && iblock < blks_used) {
		if(iblock < INODE_NUM_BLKS) {
			blk = ifile->blocks;
			tbl_pos = iblock;
		}
		
		block_read(fs->blocks, blk[tbl_pos], block);

		int start = ((pos == 0)?(offset % BLOCK_SIZE):0);
		int num = MIN(BLOCK_SIZE - start, max - pos);
		memcpy(&buffer[pos],&block[start],num);

		pos += num;
		iblock++;
	}
	*nread = pos;
	return 0;
}


int fs_write(fs_t* fs, inodeid_t file, unsigned offset, unsigned count,
   char* buffer)
{
	if (fs == NULL || file >= ITAB_SIZE || buffer == NULL) {
		dprintf("[fs_write] malformed arguments.\n");
		return -1;
	}

	if (!BMAP_ISSET(fs->inode_bmap,file)) {
		dprintf("[fs_write] inode is not being used.\n");
		return -1;
	}

	fs_inode_t* ifile = &fs->inode_tab[file];
	if (ifile->type != FS_FILE) {
		dprintf("[fs_write] inode is not a file.\n");
		return -1;
	}

	if (offset > ifile->size) {
		offset = ifile->size;
	}

	unsigned *blk;

	int blks_used = OFFSET_TO_BLOCKS(ifile->size);
	int blks_req = MAX(OFFSET_TO_BLOCKS(offset+count),blks_used)-blks_used;

	dprintf("[fs_write] count=%d, offset=%d, fsize=%d, bused=%d, breq=%d\n",
		count,offset,ifile->size,blks_used,blks_req);
	
	if (blks_req > 0) {
		if(blks_req > INODE_NUM_BLKS-blks_used) {
			dprintf("[fs_write] no free block entries in inode.\n");
			return -1;
		}

		dprintf("[fs_write] required %d blocks, used %d\n", blks_req, blks_used);

      		// check and reserve if there are free blocks
		for (int i = blks_used; i < blks_used + blks_req; i++) {

			if(i < INODE_NUM_BLKS)
				blk = &ifile->blocks[i];
	 
			if (!fsi_bmap_find_free(fs->blk_bmap,block_num_blocks(fs->blocks),blk)) { // ITAB_SIZE
				dprintf("[fs_write] there are no free blocks.\n");
				return -1;
			}
			BMAP_SET(fs->blk_bmap, *blk);
			dprintf("[fs_write] block %d allocated.\n", *blk);
		}
	}
   
	char block[BLOCK_SIZE];
	int num = 0, pos;
	int iblock = offset/BLOCK_SIZE;

   	// write within the existent blocks
	while (num < count && iblock < blks_used) {
		if(iblock < INODE_NUM_BLKS) {
			blk = ifile->blocks;
			pos = iblock;
		}
      
		block_read(fs->blocks, blk[pos], block);

		int start = ((num == 0)?(offset % BLOCK_SIZE):0);
		for (int i = start; i < BLOCK_SIZE && num < count; i++, num++) {
			block[i] = buffer[num];
		}
		block_write(fs->blocks, blk[pos], block);
		iblock++;
	}

	dprintf("[fs_write] written %d bytes within.\n", num);

  	// write within the allocated blocks
	while (num < count && iblock < blks_used + blks_req) {
		if(iblock < INODE_NUM_BLKS) {
			blk = ifile->blocks;
			pos = iblock;
		}
      
		for (int i = 0; i < BLOCK_SIZE && num < count; i++, num++) {
			block[i] = buffer[num];
		}

		block_write(fs->blocks, blk[pos], block);
		iblock++;
	}

	if (num != count) {
		printf("[fs_write] severe error: num=%d != count=%d!\n", num, count);
		exit(-1);
	}

	ifile->size = MAX(offset + count, ifile->size);

   	// update the inode in disk
	fsi_store_fsdata(fs);

	dprintf("[fs_write] written %d bytes, file size %d.\n", count, ifile->size);
	return 0;
}


int fs_create(fs_t* fs, inodeid_t dir, char* file, inodeid_t* fileid)
{	
   if (fs == NULL || dir >= ITAB_SIZE || file == NULL || fileid == NULL) {
      printf("[fs_create] malformed arguments.\n");
      return -1;
   }

   if (strlen(file) == 0 || strlen(file)+1 > FS_MAX_FNAME_SZ){
      dprintf("[fs_create] file name size error.\n");
      return -1;
   }

   if (!BMAP_ISSET(fs->inode_bmap,dir)) {
      dprintf("[fs_create] inode is not being used.\n");
      return -1;
   }

   fs_inode_t* idir = &fs->inode_tab[dir];
   if (idir->type != FS_DIR) {
      dprintf("[fs_create] inode is not a directory.\n");
      return -1;
   }

   if (fsi_dir_search(fs,dir,file,fileid) == 0) {
      dprintf("[fs_create] file already exists.\n");
      return -1;
   }
   
   // check if there are free inodes
   unsigned finode;
   if (!fsi_bmap_find_free(fs->inode_bmap,ITAB_SIZE,&finode)) {
      dprintf("[fs_create] there are no free inodes.\n");
      return -1;
   }

   // add a new block to the directory if necessary
   if (idir->size % BLOCK_SIZE == 0) {
      unsigned fblock;
      if (!fsi_bmap_find_free(fs->blk_bmap,block_num_blocks(fs->blocks),&fblock)) {
         dprintf("[fs_create] no free blocks to augment directory.\n");
         return -1;
      }
      BMAP_SET(fs->blk_bmap,fblock);
      idir->blocks[idir->size / BLOCK_SIZE] = fblock;
   }

   // add the entry to the directory
   fs_dentry_t page[DIR_PAGE_ENTRIES];
   block_read(fs->blocks,idir->blocks[idir->size/BLOCK_SIZE],(char*)page); //mete o resultado da leitura em (char*)page
   fs_dentry_t* entry = &page[idir->size % BLOCK_SIZE / sizeof(fs_dentry_t)]; // entry: próxima posição vazia do bloco
   strcpy(entry->name,file);
   entry->inodeid = finode;
   block_write(fs->blocks,idir->blocks[idir->size/BLOCK_SIZE],(char*)page);
   idir->size += sizeof(fs_dentry_t);

   // reserve and init the new file inode
   BMAP_SET(fs->inode_bmap,finode);
   fsi_inode_init(&fs->inode_tab[finode],FS_FILE);

   // save the file system metadata
   fsi_store_fsdata(fs);

   *fileid = finode;
   return 0;
}


int fs_mkdir(fs_t* fs, inodeid_t dir, char* newdir, inodeid_t* newdirid)
{
	if (fs==NULL || dir>=ITAB_SIZE || newdir==NULL || newdirid==NULL) {
		printf("[fs_mkdir] malformed arguments.\n");
		return -1;
	}

	if (strlen(newdir) == 0 || strlen(newdir)+1 > FS_MAX_FNAME_SZ){
		dprintf("[fs_mkdir] directory size error.\n");
		return -1;
	}

	if (!BMAP_ISSET(fs->inode_bmap,dir)) {
		dprintf("[fs_mkdir] inode is not being used.\n");
		return -1;
	}

	fs_inode_t* idir = &fs->inode_tab[dir];
	if (idir->type != FS_DIR) {
		dprintf("[fs_mkdir] inode is not a directory.\n");
		return -1;
	}

	if (fsi_dir_search(fs,dir,newdir,newdirid) == 0) {
		dprintf("[fs_mkdir] directory already exists.\n");
		return -1;
	}
   
   	// check if there are free inodes
	unsigned finode;
	if (!fsi_bmap_find_free(fs->inode_bmap,ITAB_SIZE,&finode)) {
		dprintf("[fs_mkdir] there are no free inodes.\n");
		return -1;
	}

   	// add a new block to the directory if necessary
	if (idir->size % BLOCK_SIZE == 0) {
		unsigned fblock;
		if (!fsi_bmap_find_free(fs->blk_bmap,block_num_blocks(fs->blocks),&fblock)) {
			dprintf("[fs_mkdir] no free blocks to augment directory.\n");
			return -1;
		}
		BMAP_SET(fs->blk_bmap,fblock);
		idir->blocks[idir->size / BLOCK_SIZE] = fblock;
	}

   	// add the entry to the directory
	fs_dentry_t page[DIR_PAGE_ENTRIES];
	block_read(fs->blocks,idir->blocks[idir->size/BLOCK_SIZE],(char*)page);
	fs_dentry_t* entry = &page[idir->size % BLOCK_SIZE / sizeof(fs_dentry_t)];
	strcpy(entry->name,newdir);
	entry->inodeid = finode;
	block_write(fs->blocks,idir->blocks[idir->size/BLOCK_SIZE],(char*)page);
	idir->size += sizeof(fs_dentry_t);

   	// reserve and init the new file inode
	BMAP_SET(fs->inode_bmap,finode);
	fsi_inode_init(&fs->inode_tab[finode],FS_DIR);

   	// save the file system metadata
	fsi_store_fsdata(fs);

	*newdirid = finode;
	return 0;
}


int fs_readdir(fs_t* fs, inodeid_t dir, fs_file_name_t* entries, int maxentries,
   int* numentries)
{
   if (fs == NULL || dir >= ITAB_SIZE || entries == NULL ||
      numentries == NULL || maxentries < 0) {
      dprintf("[fs_readdir] malformed arguments.\n");
      return -1;
   }

   if (!BMAP_ISSET(fs->inode_bmap,dir)) {
      dprintf("[fs_readdir] inode is not being used.\n");
      return -1;
   }

   fs_inode_t* idir = &fs->inode_tab[dir];
   if (idir->type != FS_DIR) {
      dprintf("[fs_readdir] inode is not a directory.\n");
      return -1;
   }

   // fill in the entries with the directory content
   fs_dentry_t page[DIR_PAGE_ENTRIES];
   int num = MIN(idir->size / sizeof(fs_dentry_t), maxentries);
   int iblock = 0, ientry = 0;

   while (num > 0) {
      block_read(fs->blocks,idir->blocks[iblock++],(char*)page);
      for (int i = 0; i < DIR_PAGE_ENTRIES && num > 0; i++, num--) {
         strcpy(entries[ientry].name, page[i].name);
         entries[ientry].type = fs->inode_tab[page[i].inodeid].type;
         ientry++;
      }
   }
   *numentries = ientry;
   return 0;
}

/*
 * void search_dir_entry (fs_t* fs, inodei_t dir, inodeid_t fileid, int *num_dir_entry, char*page)
 *
 * searches an entry in a file system
 *
 * fs - reference to the filesystem
 * dir - directory to be searched
 * fileid - file id 
 * *num_dir_entry - number refering to the position of an entry in a block of a certain directory [out]
 * *page - array of entries [out]
 */
void search_dir_entry (fs_t* fs, inodeid_t dir, inodeid_t fileid, int *num_dir_entry, int* block_num, char* page){
	
	fs_inode_t* idir = &fs->inode_tab[dir];
	int num = idir->size / sizeof(fs_dentry_t); //número de entradas no directório
	int iblock = 0;
	fs_dentry_t ipage[DIR_PAGE_ENTRIES];

	while (num > 0){
		block_read(fs->blocks, idir->blocks[iblock++], (char*)ipage); //guarda um bloco em page
		for (int i = 0; i < DIR_PAGE_ENTRIES && num > 0; i++, num--) {
			if (ipage[i].inodeid == fileid){
				num_dir_entry = &i;
				block_num = &iblock;
				page = (char *) ipage;
				return;
			}
		}
	}

	return;
}

void fs_remove_file(fs_t* fs, inodeid_t entryid) {

	fs_inode_t ifile = fs->inode_tab[entryid];

	for(int i = 0; i < INODE_NUM_BLKS && ifile.blocks[i] != 0 ; i++) {
		BMAP_CLR(fs->blk_bmap, ifile.blocks[i]);
	}

	BMAP_CLR(fs->inode_bmap, entryid);
}



void fs_remove_dir(fs_t* fs, inodeid_t dir) {

	fs_inode_t idir = fs->inode_tab[dir];
	int num = idir.size / sizeof(fs_dentry_t); // numero de entradas num directorio
	fs_dentry_t page[DIR_PAGE_ENTRIES];

	for(int i = 0; i < INODE_NUM_BLKS && idir.blocks[i] != 0; i++){
		int num_dir_pg_entries;
		while(num > 0){
			block_read(fs->blocks, idir.blocks[i], (char*)page);
			for(num_dir_pg_entries = 0; num_dir_pg_entries < DIR_PAGE_ENTRIES && num > 0; i++, num --){
				inodeid_t entryid = page[num_dir_pg_entries].inodeid;
				if (fs->inode_tab[entryid].type == FS_FILE)
					fs_remove_file(fs, entryid);
				else fs_remove_dir(fs, entryid);
			}
		}
	}

	BMAP_CLR(fs->inode_bmap, dir);

}


	

int fs_remove(fs_t* fs, inodeid_t dir, char *name)
{
	//checks if the arguments are valid
	if (fs == NULL || dir>=ITAB_SIZE || name == NULL) {
		dprintf("[fs_remove] malformed arguments. \n");
		return -1;
	}

	if (strlen(name) == 0 || strlen(name)+1 > FS_MAX_FNAME_SZ) {
		dprintf("[fs_remove] file name size error\n");
		return -1;
	}

	if (!BMAP_ISSET(fs->inode_bmap, dir)) {
		dprintf("[fs_remove] inode is not being used.\n");
		return -1;
	}
	
	inodeid_t entryid = 0;
	if (!fsi_dir_search(fs, dir, name, &entryid) == 0) {
		dprintf("[fs_remove] file/dir does not exist\n");
		return -1;
	}
	
	fs_inode_t* idir = &fs->inode_tab[dir];
	if (idir->type != FS_DIR) {
		dprintf("[fs_create] file already exists. \n");
		return -1;
	}

	fs_dentry_t page[DIR_PAGE_ENTRIES];	
	int num_dir_entry = 0, block_num = 0;
	
	search_dir_entry(fs, dir, entryid, &num_dir_entry, &block_num, (char*)page);
		
	fs_dentry_t last_page[DIR_PAGE_ENTRIES]; // array de entradas do ultimo bloco
	int last_entry_index = (idir->size % BLOCK_SIZE / sizeof(fs_dentry_t)) -1;
	block_read(fs->blocks,idir->blocks[idir->size/BLOCK_SIZE],(char *)last_page);
	fs_dentry_t* last_entry	= &last_page[last_entry_index]; // ponteiro p/ última posição ocupada do dir
	

	page[num_dir_entry] = *last_entry; // mete a ultima entrada na posição da que vai ser removida
	block_write(fs->blocks, idir->blocks[block_num], (char*)page); // escreve o bloco (page) no disco
	idir->size -= sizeof(fs_dentry_t); // diminui o tamanho do directório em uma entrada

	int i, j;

	if (last_entry_index == 0) { //se for a primeira entrada do bloco
		for(i = 0, j = 1; idir->blocks[j] != 0; i++, j++);
		BMAP_CLR(fs->blk_bmap, idir->blocks[i]); 
		idir->blocks[i] = 0;
	}


	fs_inode_t ientry = fs->inode_tab[entryid]; // vai buscar o inode do ficheiro/directorio à tabela de inodes
	
	if(ientry.type == FS_FILE)
		fs_remove_file(fs, entryid);
	else 
		fs_remove_file(fs, entryid);
	
	BMAP_CLR(fs->inode_bmap, entryid);

	return 0;
}

void fs_dump(fs_t* fs)
{
   printf("Free block bitmap:\n");
   fsi_dump_bmap(fs->blk_bmap,BLOCK_SIZE);
   printf("\n");
   
   printf("Free inode table bitmap:\n");
   fsi_dump_bmap(fs->inode_bmap,BLOCK_SIZE);
   printf("\n");
}

 
