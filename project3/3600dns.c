/*
 * Ben Lieberman
 * CCIS: bliebs
 * Team: blieberman
 * CS3600, Spring 2014
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

#define MAX_ARG_LEN 39 // IPv6 addresses are 39 bytes
#define HEADER_ID 1337
#define DEBUG 1 // toggle 1 for on and 0 for off

/* converts name to proper packet format (i.e., www.google.com . 3www6google3com0)
 * numbers indicate the #of bytes to follow, and replace all periods (to separate
 * top-level, second-level, etc. domains) the \0 appended to the end is a signal
 * meaning “end of name”.
 */
static void convert_name(char *name) {
  char *buffer = malloc(16*sizeof(int)); //buffer to copy tok
  char toksize[20] = "\0"; // size of the tok to append on buffer
  char *tok;
  char converted_name[strlen(name) + 2];
  int size = 0;

  tok = strtok(name, "."); // split on each "."
  size = strlen(tok);
  sprintf(toksize, "%c", size);
  strcat(buffer, toksize); //cat size
  strcat(buffer, tok); //cat string

  while ( tok = strtok(NULL, ".") ) { // go through the whole string
    size = 0;
    size = strlen(tok);
    sprintf(toksize, "%c", size); //size to string form (kind of nasty)
    strcat(buffer, toksize); //cat size
    strcat(buffer, tok); //cat string

    //DEBUG: fprintf(stderr, "buffer: %s\n", buffer);
 }
 strcat(buffer, "\0"); //cat final zero to imply end of string
 strcpy(name, buffer);
 free(buffer);
}

/* parses question name to readable format;
 * takes in destination, source and position variables
 */
void parse_qname(char* qname, char* source, int offset) {
  //3www6goolgle3com -> www.google.com
  
  //to debug along the way of development
  //fprintf(stderr, "qname before: %s\n", qname);
  
  int label_len = 0;
  label_len = source[offset]; //length designated by start label
  int curpos = offset + 1; // current position after label
  
  int qindex = 0;
  
  for(char a = 0; source[curpos] != a; curpos++) { // loop through until zero byte
    unsigned char curchar = source[curpos]; // pop top char from buffer
    if ((int)curchar > 0 && (int)curchar <= 9) { // is the char a number?
      qname[qindex] = '.'; // cat . between labels
      qindex++; // advance qname index
    }
    else { // else just copy over char in current position
      qname[qindex] = curchar;
      qindex++;  // advance index
    }
  }
}

/**
 * This function will print a hex dump of the provided packet to the screen
 * to help facilitate DEBUGging.  In your milestone and final submission, you 
 * MUST call dump_packet() with your packet right before calling sendto().
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
  /**
   * process the arguments:
   * ./3600dns @<ip_address:port> <name>
   * port is optional with default value of 53
   */

  short port = 53; // UDP port number of DNS Server
  char *ip_address = malloc((MAX_ARG_LEN + 1) * sizeof(char));
  char *name = malloc((MAX_ARG_LEN + 1) * sizeof(char));

  if (argc != 3) {
    fprintf(stderr, "usage: %s @<ip_address:port> <name>\n", argv[0]);
    exit(1);
  }
  else {
    if (argv[1][0] == '@') { // skipping argv[0]
      strcpy(name, argv[2]); // copy argv[2] to name

      int colonpos = 0; // position of colon for port in first argument
      char *colon; // buffer for strchr

      /* search for the optional ":" in first argument */
      colon = strchr(argv[1], ':');

      if (colon) {
        char buf[6] = "\0";

        colonpos = (int)(colon - argv[1]);
        strncat(ip_address, argv[1]+1, colonpos-1); // copy after "@" and before ":"
        strncat(buf, argv[1]+colonpos+1, 3); // copy after ":" to end of char*

        port = atoi(buf); // atoi char* to int
      }
      else {
        strcpy(ip_address, argv[1]+1); // if no port just copy whole argv[1]
      }
    }
    else {
      fprintf(stderr, "usage: %s @<ip_address:port> <name>\n", argv[0]);
    }
  }

  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "ip_address: %s\n", ip_address);
    fprintf(stderr, "port: %i\n", port);
    fprintf(stderr, "name: %s\n", name);
  }
  ///////////////

  // convert name to proper dns packet format
  convert_name(name);

  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "converted name: %s\n", name);
  }
  ///////////////

  // construct the DNS request
  header sen_header; // initialize header
  // set sen_header fields
  sen_header.id = htons(HEADER_ID);
  sen_header.rd = 1;
  sen_header.tc = 0;
  sen_header.aa = 0;
  sen_header.opcode = 0;
  sen_header.qr = 0;
  sen_header.rcode = 0;
  sen_header.ra = 0;
  sen_header.z = 0;
  sen_header.qdcount = htons(1);
  sen_header.ancount = htons(0);
  sen_header.nscount = htons(0);
  sen_header.arcount = htons(0);

  question my_question; // initialize question
  // set my_question fields
  my_question.qtype = htons(1);
  my_question.qclass = htons(1);

  answer my_answer; // initialize answer

  char pckt_buffer[65536]; /* /48 network prefix allows 65536 */
  int nl = strlen(name); //name length

  /* copy into buffer */
  memset(pckt_buffer, 0, 65536); // buffer gets zero'd out
  memcpy(&pckt_buffer, &sen_header, sizeof(header));
  memcpy(&pckt_buffer[sizeof(header)], name, nl);
  memcpy(&pckt_buffer[1 + sizeof(header) + nl], &my_question, sizeof(question));

  int pckt_len = sizeof(header) + 1 + nl + sizeof(question); // packet buffer len

  // send the DNS request (and call dump_packet with your request)
  dump_packet(pckt_buffer, pckt_len);

  // first, open a UDP socket
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // next, construct the destination address
  struct sockaddr_in out;
  out.sin_family = AF_INET;
  out.sin_port = htons(port);
  out.sin_addr.s_addr = inet_addr(ip_address);

  if (sendto(sock, pckt_buffer, pckt_len, 0, &out, sizeof(out)) < 0) {
    fprintf(stderr, "An error occured while sending the packet.\n");
    return -1;
  }

  // important variables get initialized again for reuse for response
  memset(pckt_buffer, 0, 65536);
  //memset(&my_question, 0, sizeof(question));
  //memset(&sen_header, 0, sizeof(header));
  //pckt_len = 0;

  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "packet cleared output:\n");
    dump_packet(pckt_buffer, pckt_len);
  }
  ///////////////

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
    if (recvfrom(sock, pckt_buffer, pckt_len, 0, &in, &in_len) < 0) {
       fprintf(stderr, "An error occured receiving response.\n");
       return -1;
    }
  } else {
     fprintf(stderr, "A timeout occured receiving response.\n");
     return -2;
  }

  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "packet response output:\n");
    dump_packet(pckt_buffer, pckt_len);
  }
  ///////////////

  // create received header struct
  header rec_header;

  /////* parse response packet header */////
  // copy header from packet buffer
  memcpy(&rec_header, &pckt_buffer, sizeof(header));

  // verify header fields and check for errors
  if (ntohs(rec_header.id) != HEADER_ID ||
    rec_header.qr != 1 ||
    rec_header.tc != 0 ||
    rec_header.ra != 1 ||
    rec_header.rd != 1) {
      fprintf(stderr, "Response packet header has errors.\n");
      return -1;
  }

  pckt_len = sizeof(rec_header);
  int answercnt = ntohs(rec_header.ancount); //number of answers
  // allocate sizeof sent name + 2
  unsigned char* rec_qname = (char *) malloc(2 + strlen(name));
  
  // parse received packet header qname
  parse_qname(rec_qname, pckt_buffer, pckt_len);

  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "answer count: %i\n", answercnt);
    fprintf(stderr, "rec_qname after: %s\n", rec_qname);
    fprintf(stderr, "sizeof rec_qname: %i\n", sizeof(rec_qname));
  }
  ///////////////

  free(ip_address);
  free(name);
  //free(rec_qname);
  return 0;
}