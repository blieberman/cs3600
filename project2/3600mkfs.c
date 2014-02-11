/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 * This program is intended to format your disk file, and should be executed
 * BEFORE any attempt is made to mount your file system.  It will not, however
 * be called before every mount (you will call it manually when you format 
 * your disk file).
 */
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "3600fs.h"
#include "disk.h"
//magic number is 7

//struct for the VCB, first block of the file system
typedef struct vcb_s {
        // a magic number to identify your disk
        int magic;
        
        // description of the disk layout
        int blocksize;
        int de_start;
        int de_length;
        int fat_start;
        int fat_length;
        int db_start;
        
        // metadata for the root directory
        uid_t user;
        gid_t group;
        mode_t mode;
        struct timespec access_time;
        struct timespec modify_time;
        struct timespec create_time;
} vcb;

// struct for Directory Entries
typedef struct dirent_s {
        unsigned int valid;
        unsigned int first_block;
        unsigned int size;
        uid_t user;
        gid_t group;
        mode_t mode;
        struct timespec access_time;
        struct timespec modify_time;
        struct timespec create_time;
        char name[];
} dirent;

// struct for FAT
typedef struct fatent_s {
        unsigned int used:1; 
        unsigned int eof:1;
        unsigned int next:30;
} fatent;


void myformat(int size) {
  // Do not touch or move this function
  dcreate_connect();

  /* 3600: FILL IN CODE HERE.  YOU SHOULD INITIALIZE ANY ON-DISK
           STRUCTURES TO THEIR INITIAL VALUE, AS YOU ARE FORMATTING
           A BLANK DISK.  YOUR DISK SHOULD BE size BLOCKS IN SIZE. */
           
  //Volume Control Block First
  // set up VCB
  vcb myvcb;
  myvcb.magic = 7;
  myvcb.blocksize = BLOCKSIZE;
  myvcb.de_start = 1;
  myvcb.de_length = size;
  myvcb.fat_start = myvcb.de_start + size;
  myvcb.fat_length = size;
  myvcb.db_start = myvcb.fat_start + size;
  //myvcb.user = getuid();
  //myvcb.group = getgid();
  myvcb.mode = 0777;
  //clock_gettime(CLOCK_REALTIME, &myvcb.access_time);
  //myvcb.modify_time = myvcb.access_time;
  //myvcb.create_time = myvcb.access_time;
  
  // copy vcb to a BLOCKSIZE-d location
  char temp [BLOCKSIZE];
  memset(temp, 0, BLOCKSIZE);
  memcpy(temp, &myvcb, sizeof(vcb));
  //finally actually write it to disk in the 0th block
  dwrite(0, temp);
  /*
  // Directory Entry Blocks
  // create a dirent
  dirent mydirent;
  mydirent.valid = 7;
  mydirent.first_block = 1;
  mydirent.size = BLOCKSIZE;
  mydirent.user = getuid();
  mydirent.group = getgid();
  mydirent.mode = 0777;
  clock_gettime(CLOCK_REALTIME, mydirent.acces_time);
  myvcb.modify_time = mydirent.acces_time;
  myvcb.create_time = mydirent.acces_time;
  
  int i = myvcb.de_start;
  // make all DE's equal to the same dirent
  for(i; i < myvcb.de_start + myvcb.de_length; i++) {
         memset(temp, 0, BLOCKSIZE);
         memcpy(temp, &mydirent, sizeof(dirent);
         dwrite(i, temp);
  }
        
  // File Allocation Tables
  
  
  
  // Data Blocks

   3600: AN EXAMPLE OF READING/WRITING TO THE DISK IS BELOW - YOU'LL
           WANT TO REPLACE THE CODE BELOW WITH SOMETHING MEANINGFUL. 
          
  // first, create a zero-ed out array of memory  
  char *tmp = (char *) malloc(BLOCKSIZE);
  memset(tmp, 0, BLOCKSIZE);

  // now, write that to every block
  for (int i=0; i<size; i++) 
    if (dwrite(i, tmp) < 0) 
      perror("Error while writing to disk");

  // voila! we now have a disk containing all zeros
  */
  // Do not touch or move this function
  dunconnect();
}

int main(int argc, char** argv) {
  // Do not touch this function
  if (argc != 2) {
    printf("Invalid number of arguments \n");
    printf("usage: %s diskSizeInBlockSize\n", argv[0]);
    return 1;
  }

  unsigned long size = atoi(argv[1]);
  printf("Formatting the disk with size %lu \n", size);
  myformat(size);
}

