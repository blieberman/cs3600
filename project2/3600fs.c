/*
 * CS3600, Spring 2014
 * Project 2
 *
 * This file contains all of the basic functions that you will need 
 * to implement for this project.  Please see the project handout
 * for more details on any particular function, and ask on Piazza if
 * you get stuck.
 */

#define FUSE_USE_VERSION 26

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#define _POSIX_C_SOURCE 199309

#include <time.h>
#include <fuse.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <sys/statfs.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include "3600fs.h"
#include "disk.h"

/*
 * Initialize filesystem. Read in file system metadata and initialize
 * memory structures. If there are inconsistencies, now would also be
 * a good time to deal with that. 
 *
 * HINT: You don't need to deal with the 'conn' parameter AND you may
 * just return NULL.
 *
 */
static void* vfs_mount(struct fuse_conn_info *conn) {
  fprintf(stderr, "vfs_mount called\n");

  // Do not touch or move this code; connects the disk
  dconnect();

  /* 3600: YOU SHOULD ADD CODE HERE TO CHECK THE CONSISTENCY OF YOUR DISK
           AND LOAD ANY DATA STRUCTURES INTO MEMORY */
           
  //allocate your VCB
  vcb myvcb;
  //some error catching on the VCB
  if (dread(0, &myvcb) < 0)
    perror("Error while reading VCB");

  // invalid magic number
  if (myvcb.magic != MAGICNUM) { 
    fprintf(stderr, "Error: Unrecognized ID.\n");
  }
  // Disk was not unmounted cleanly last time.
  else if (myvcb.dirty) { 
    fprintf(stderr, "Error: Disk was not unmounted properly.\n");
  }
  
  // disk is dirty until it is unmounted.
  myvcb.dirty = 1;
  
  //write modified vcb back to disk
  if(dwrite(0, &myvcb) < 0) {
    perror("Error while writing VCB");
  }

  return NULL;
}

/*
 * Called when your file system is unmounted.
 *
 */
static void vfs_unmount (void *private_data) {
  fprintf(stderr, "vfs_unmount called\n");

  /* 3600: YOU SHOULD ADD CODE HERE TO MAKE SURE YOUR ON-DISK STRUCTURES
           ARE IN-SYNC BEFORE THE DISK IS UNMOUNTED (ONLY NECESSARY IF YOU
           KEEP DATA CACHED THAT'S NOT ON DISK */
           
           
           
  // Read VCB from disk.
  vcb myvcb;
  if (dread(0, &myvcb) < 0) {
    perror("Error while reading VCB");
  }

  // set dirty to 0 to show success in unmounting
  myvcb.dirty = 0;

  // write modified VCB back to the disk
  if (dwrite( 0, &myvcb) < 0) {
    perror("Error while writing VCB");
  }


  // Do not touch or move this code; unconnects the disk
  dunconnect();
}

/* 
 *
 * Given an absolute path to a file/directory (i.e., /foo ---all
 * paths will start with the root directory of the CS3600 file
 * system, "/"), you need to return the file attributes that is
 * similar stat system call.
 *
 * HINT: You must implement stbuf->stmode, stbuf->st_size, and
 * stbuf->st_blocks correctly.
 *
 */
static int vfs_getattr(const char *path, struct stat *stbuf) {
  fprintf(stderr, "vfs_getattr called\n");

  // Do not mess with this code 
  stbuf->st_nlink = 1; // hard links
  stbuf->st_rdev  = 0;
  stbuf->st_blksize = BLOCKSIZE;

  /* 3600: YOU MUST UNCOMMENT BELOW AND IMPLEMENT THIS CORRECTLY */
  vcb myvcb;
  dread(0, &myvcb);
  
  int current = 0;
  int found = 0;
  path++;
  //path is the root directory
  if (strcmp(path, "") == 0) {
    
    stbuf->st_mode  = 0777 | S_IFDIR;
  }
  else {
    
    for (current = myvcb.de_start; current < myvcb.de_start + myvcb.de_length; current++) {
      dirent tmp;
      dread(current, &tmp);
      if (strcmp(tmp.name, path) == 0) {
        stbuf->st_mode  = tmp.mode | S_IFREG;
        found = 1;
        break;
      }
      
    }
    if (found == 0) {
      return -ENOENT;
    }
  
    
  }  
    

  stbuf->st_uid     = current.user;
  stbuf->st_gid     = current.group;
  stbuf->st_atime   = current.access_time; 
  stbuf->st_mtime   = current.modify_time;
  stbuf->st_ctime   = current.create_time;
  stbuf->st_size    = current.size;
  stbuf->st_blocks  = current.size / BLOCKSIZE;
  

  return 0  ;
}

/*
 * Given an absolute path to a directory (which may or may not end in
 * '/'), vfs_mkdir will create a new directory named dirname in that
 * directory, and will create it with the specified initial mode.
 *
 * HINT: Don't forget to create . and .. while creating a
 * directory.
 */
/*
 * NOTE: YOU CAN IGNORE THIS METHOD, UNLESS YOU ARE COMPLETING THE 
 *       EXTRA CREDIT PORTION OF THE PROJECT.  IF SO, YOU SHOULD
 *       UN-COMMENT THIS METHOD.
static int vfs_mkdir(const char *path, mode_t mode) {

  return -1;
} */

/** Read directory
 *
 * Given an absolute path to a directory, vfs_readdir will return 
 * all the files and directories in that directory.
 *
 * HINT:
 * Use the filler parameter to fill in, look at fusexmp.c to see an example
 * Prototype below
 *
 * Function to add an entry in a readdir() operation
 *
 * @param buf the buffer passed to the readdir() operation
 * @param name the file name of the directory entry
 * @param stat file attributes, can be NULL
 * @param off offset of the next entry or zero
 * @return 1 if buffer is full, zero otherwise
 * typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
 *                                 const struct stat *stbuf, off_t off);
 *			   
 * Your solution should not need to touch fi
 *
 */
static int vfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
  
  //reading root directory
  if(strcmp(path, "/") == 0){
    //read in VCB
      vcb myvcb;
      char temp_vb[BLOCKSIZE];
      memset(temp_vb, 0, BLOCKSIZE);
      dread(0, temp_vb);
      memcpy(&myvcb, temp_vb, sizeof(vcb));
  
  
      for(int i = myvcb.de_start; i < myvcb.de_start+myvcb.de_length; i++){
         dirent de;
         char temp_de[BLOCKSIZE];
         memset(temp_de, 0, BLOCKSIZE);
         dread(i, temp_de);
         memcpy(&de, temp_de, sizeof(de));
      
         if(filler(buf, de.name, NULL, 0) != 0){
	           return -ENOMEM;
         }
      }
      return 0;
  }
  else{
      return -1;
  }
}


/*
 * Given an absolute path to a file (for example /a/b/myFile), vfs_create 
 * will create a new file named myFile in the /a/b directory.
 *
 */
static int vfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    //make sure the file doesn't already exist
    int current = 1;
    for (current; current < myvcb.de_start + myvcb.de_length; current++) {
      dirent dt;
      dread(current, &dt);
      if (strcmp(dt.name, path) == 0) {
        return -EEXIST;
      }
    }
    
    //make sure there is room for the new file
    int empty = -1;
    for (current = 1; i < myvcb.de_length; i++) {
      dirent dr;
      dread(current,&dr);
      if(dr.valid == 0){
        empty = current;
        break;
      }
    }
    
    //if no room left, error
    if (empty == -1) {
      return -1;
    }
    
    // otherwise, create the dirent
    dirent de;
    
    
    de.valid = 1;
    de.user = geteuid();
    de.group = getegid();
    de.mode = mode;
    clock_gettime(CLOCK_REALTIME, &de.access_time);
    clock_gettime(CLOCK_REALTIME, &de.modify_time);
    clock_gettime(CLOCK_REALTIME, &de.create_time);
    strcpy(de.name, path);
    dwrite(empty, &de);
    
    return 0;
}

/*
 * The function vfs_read provides the ability to read data from 
 * an absolute path 'path,' which should specify an existing file.
 * It will attempt to read 'size' bytes starting at the specified
 * offset (offset) from the specified file (path)
 * on your filesystem into the memory address 'buf'. The return 
 * value is the amount of bytes actually read; if the file is 
 * smaller than size, vfs_read will simply return the most amount
 * of bytes it could read. 
 *
 * HINT: You should be able to ignore 'fi'
 *
 */
static int vfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
  
  int bytes = 0;
  vcb myvcb;
  dread(0, &myvcb);
  int current;
  for (current = myvcb.de_start; current < myvcb.de_start + myvcb.de_length; current++) {
    dirent de;
    dread(current, &de);
    
    if (strcmp(path, de.name) == 0) {
      if (size + offset > de.size) {
        bytes = de.size - offset;
      }
      
      if(offset % BLOCKSIZE + bytes <= BLOCKSIZE) {
        char temp[BLOCKSIZE];
        memset(temp, 0, BLOCKSIZE);
        dread(myvcb.db_start + de.first_block, temp);
        memcpy(buf, temp + offset % BLOCKSIZE, bytes);
      }
      return bytes;
    }
  }
  return -1;

  
}

/*
 * The function vfs_write will attempt to write 'size' bytes from 
 * memory address 'buf' into a file specified by an absolute 'path'.
 * It should do so starting at the specified offset 'offset'.  If
 * offset is beyond the current size of the file, you should pad the
 * file with 0s until you reach the appropriate length.
 *
 * You should return the number of bytes written.
 *
 * HINT: Ignore 'fi'
 */
static int vfs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{

  /* 3600: NOTE THAT IF THE OFFSET+SIZE GOES OFF THE END OF THE FILE, YOU
           MAY HAVE TO EXTEND THE FILE (ALLOCATE MORE BLOCKS TO IT). */

  return 0;
}

/**
 * This function deletes the last component of the path (e.g., /a/b/c you 
 * need to remove the file 'c' from the directory /a/b).
 */
static int vfs_delete(const char *path)
{

  /* 3600: NOTE THAT THE BLOCKS CORRESPONDING TO THE FILE SHOULD BE MARKED
           AS FREE, AND YOU SHOULD MAKE THEM AVAILABLE TO BE USED WITH OTHER FILES */

    return 0;
}

/*
 * The function rename will rename a file or directory named by the
 * string 'oldpath' and rename it to the file name specified by 'newpath'.
 *
 * HINT: Renaming could also be moving in disguise
 *
 */
static int vfs_rename(const char *from, const char *to)
{

    return 0;
}


/*
 * This function will change the permissions on the file
 * to be mode.  This should only update the file's mode.  
 * Only the permission bits of mode should be examined 
 * (basically, the last 16 bits).  You should do something like
 * 
 * fcb->mode = (mode & 0x0000ffff);
 *
 */
static int vfs_chmod(const char *file, mode_t mode)
{

    return 0;
}

/*
 * This function will change the user and group of the file
 * to be uid and gid.  This should only update the file's owner
 * and group.
 */
static int vfs_chown(const char *file, uid_t uid, gid_t gid)
{

    return 0;
}

/*
 * This function will update the file's last accessed time to
 * be ts[0] and will update the file's last modified time to be ts[1].
 */
static int vfs_utimens(const char *file, const struct timespec ts[2])
{

    return 0;
}

/*
 * This function will truncate the file at the given offset
 * (essentially, it should shorten the file to only be offset
 * bytes long).
 */
static int vfs_truncate(const char *file, off_t offset)
{

  /* 3600: NOTE THAT ANY BLOCKS FREED BY THIS OPERATION SHOULD
           BE AVAILABLE FOR OTHER FILES TO USE. */

    return 0;
}


/*
 * You shouldn't mess with this; it sets up FUSE
 *
 * NOTE: If you're supporting multiple directories for extra credit,
 * you should add 
 *
 *     .mkdir	 = vfs_mkdir,
 */
static struct fuse_operations vfs_oper = {
    .init    = vfs_mount,
    .destroy = vfs_unmount,
    .getattr = vfs_getattr,
    .readdir = vfs_readdir,
    .create	 = vfs_create,
    .read	 = vfs_read,
    .write	 = vfs_write,
    .unlink	 = vfs_delete,
    .rename	 = vfs_rename,
    .chmod	 = vfs_chmod,
    .chown	 = vfs_chown,
    .utimens	 = vfs_utimens,
    .truncate	 = vfs_truncate,
};

int main(int argc, char *argv[]) {
    /* Do not modify this function */
    umask(0);
    if ((argc < 4) || (strcmp("-s", argv[1])) || (strcmp("-d", argv[2]))) {
      printf("Usage: ./3600fs -s -d <dir>\n");
      exit(-1);
    }
    return fuse_main(argc, argv, &vfs_oper, NULL);
}
