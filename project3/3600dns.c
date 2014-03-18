/*
 * CS3600, Spring 2014
 * Project 3 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "3600dns.h"

#define MAX_ARG_LEN 64

/**
 * This function will print a hex dump of the provided packet to the screen
 * to help facilitate debugging.  In your milestone and final submission, you 
 * MUST call dump_packet() with your packet right before calling sendto().  
 * You're welcome to use it at other times to help debug, but please comment those
 * out in your submissions.
 *
 * DO NOT MODIFY THIS FUNCTION
 *
 * data - The pointer to your packet buffer
 * size - The length of your packet
 */
static void dump_packet(unsigned char *data, int size) {
    unsigned char *p = data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for(n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.4x",
               ((unsigned int)p-(unsigned int)data) );
        }
            
        c = *p;
        if (isprint(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if(n%16 == 0) { 
            /* line completed */
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if(n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}

int main(int argc, char *argv[]) {
  int debug = 1; // debug mode is true with 1
  /**
   * I've included some basic code for opening a socket in C, sending
   * a UDP packet, and then receiving a response (or timeout).  You'll 
   * need to fill in many of the details, but this should be enough to
   * get you started.
   */

  /**
   * process the arguments:
   * ./3600dns @<server:port> <name>
   * port is optional with default value of 53
   */
  
  // String version of UDP port number of the DNS server.
  int port = 53;
  // The IP address of the DNS server, in a.b.c.d format.
  char server[MAX_ARG_LEN] = "\0";
  // The name to query for
  char name[MAX_ARG_LEN] = "\0";
    
  if (argc != 3) {
    fprintf(stderr, "usage: %s @<server:port> <name>\n", argv[0]);
    exit(1);
  }
  else {  
    if (argv[1][0] == '@') { // skipping argv[0]
      strcpy(name, argv[2]); //copy argv[2] to name
      
      // for debugging
      if (debug == 1) {
        fprintf(stderr, "server: %s\n", server);
        fprintf(stderr, "port: %i\n", port);
        fprintf(stderr, "name: %s\n", name);
      }
      
      int colonpos = 0; // position of colon for port in first argument
      char *colon; // buffer for strchr
      
      /* search for the optional ":" in first argument */
      colon = strchr(argv[1], ':');
      
      if (colon) {
        char buf[3] = "\0";
        
        colonpos = (int)(colon - argv[1]);
        strncat(server, argv[1]+1, colonpos-1); // copy after "@" and before ":"
        strncat(buf, argv[1]+colonpos+1, 3); // copy after ":" to end of char*
        
        port = atoi(buf); // atoi char* to int
      }
      else {
        strcpy(server, argv[1]+1); // if no port just copy whole argv[1]
      }
    }
    else {
      fprintf(stderr, "usage: %s @<server:port> <name>\n", argv[0]);
    }
  }
  
  // for debugging
  if (debug == 1) {
    fprintf(stderr, "server: %s\n", server);
    fprintf(stderr, "port: %i\n", port);
    fprintf(stderr, "name: %s\n", name);
  }

  /*

  // construct the DNS request

  // send the DNS request (and call dump_packet with your request)
  
  // first, open a UDP socket  
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // next, construct the destination address
  struct sockaddr_in out;
  out.sin_family = AF_INET;
  out.sin_port = htons(<<DNS server port number, as short>>);
  out.sin_addr.s_addr = inet_addr(<<DNS server IP as char*>>);

  if (sendto(sock, <<your packet>>, <<packet len>>, 0, &out, sizeof(out)) < 0) {
    // an error occurred
  }

  // wait for the DNS reply (timeout: 5 seconds)
  struct sockaddr_in in;
  socklen_t in_len;

  // construct the socket set
  fd_set socks;
  FD_ZERO(&socks);
  FD_SET(sock, &socks);

  // construct the timeout
  struct timeval t;
  t.tv_sec = 5; //5 second timeout as specificied
  t.tv_usec = 0;

  // wait to receive, or for a timeout
  if (select(sock + 1, &socks, NULL, NULL, &t)) {
    if (recvfrom(sock, <<your input buffer>>, <<input len>>, 0, &in, &in_len) < 0) {
      // an error occured
    }
  } else {
    // a timeout occurred
  }

  // print out the result
  */
  
  return 0;
}