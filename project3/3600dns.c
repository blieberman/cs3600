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
#define DEBUG 0 // toggle 1 for on and 0 for off

/* converts name to proper packet format (i.e., www.google.com . 3www6google3com0)
 * numbers indicate the #of bytes to follow, and replace all periods (to separate
 * top-level, second-level, etc. domains) the \0 appended to the end is curchar signal
 * meaning “end of name”.
 */
static void nameToDNS(char *name) {
  char *buffer = malloc(16*sizeof(int)); // buffer to copy tok
  char toksize[20] = "\0"; // size of the tok to append on buffer
  char *tok;
  int size = 0;

  tok = strtok(name, "."); // split on each "."
  size = strlen(tok);
  sprintf(toksize, "%c", size);
  strcat(buffer, toksize); // cat size
  strcat(buffer, tok); // cat string

  while ( (tok = strtok(NULL, ".")) ) { // go through the whole string
    size = 0;
    size = strlen(tok);
    sprintf(toksize, "%c", size); // size to string form (kind of nasty)
    strcat(buffer, toksize); // cat size
    strcat(buffer, tok); // cat string

    //DEBUG: fprintf(stderr, "buffer: %s\n", buffer);
 }
 strcat(buffer, "\0"); // cat final zero to imply end of string
 strcpy(name, buffer);
 free(buffer);
}

int DNSToName(unsigned char* qname, unsigned char* source, int offset) {
  //3www6goolgle3com -> www.google.com
  
  int curpos = offset + 1; // current position after label
  int qindex = 0; // index for qname
  int finlen = 0; // count for final length
  
  unsigned char curchar = source[curpos]; // pop top char from buffer
    
  while(curchar != 0) {
    /* check for compression */
    if ((curchar & 0xc0) == 0xc0) { // 0xc0 = 0b11000000
      unsigned short ptr = (source[offset] << 8) + source[offset + 1];
      ptr &= 0x3fff; // 0x3fff = 0b11111111111111
      curpos = ptr;
    }
    if ((int)curchar > 0 && (int)curchar <= 9) { // is the char a number?
      qname[qindex] = '.'; // cat . between labels
      qindex++; // advance qname index
    }
    else { // else just copy over char in current position
      qname[qindex] = curchar;
      qindex++;  // advance index
    }
    finlen++;
    curpos++;
    curchar = source[curpos];
  }
  //// DEBUG ////
  if (DEBUG == 1) {
    printf("finlen: %i\n", finlen);
    printf("curpos: %i\n", curpos);
    printf("offset: %i\n", offset);
  }
  ///////////////
  //strcat(qname, "\0");
  return finlen;
}

/* converts dns encoded ip address to decimal ip address */
int DNSToIP(unsigned char* rd, unsigned char* source, int offset) {
  unsigned char curchar = source[offset];
  int position = offset + 1;
  unsigned char segs[4];
  
  for (int i = 0; i < 4; i++) {
    segs[i] = curchar; // IP address segments
    curchar = source[position];
    position++;
  }
  // save in decimal form
  sprintf((char*)rd,"%d.%d.%d.%d", segs[0], segs[1], segs[2], segs[3]);
  // always return four due to four segments
  return 4;
}

/**
 * This function will print curchar hex dump of the provided packet to the screen
 * to help facilitate debugging.  In your milestone and final submission, you 
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
      exit(1);
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
  nameToDNS(name);

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
  sen_header.qdcount = htons(0x0001);
  sen_header.ancount = htons(0);
  sen_header.nscount = htons(0);
  sen_header.arcount = htons(0);

  question sen_question; // initialize question
  // set sen_question fields
  sen_question.qtype = htons(0x0001);
  sen_question.qclass = htons(0x0001);

  unsigned char pckt_buffer[65536]; /* /48 network prefix allows 65536 */
  int nl = strlen(name); //name length

  /* copy into buffer */
  memset(pckt_buffer, 0, 65536); // buffer gets zero'd out
  memcpy(&pckt_buffer, &sen_header, sizeof(header));
  memcpy(&pckt_buffer[sizeof(header)], name, nl);
  memcpy(&pckt_buffer[1 + sizeof(header) + nl], &sen_question, sizeof(question));

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
    fprintf(stderr, "Error: a problem occured while sending the packet.\n");
    return -1;
  }

  // buffer gets initialized again for reuse for response
  memset(pckt_buffer, 0, 65536);

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
    if (recvfrom(sock, pckt_buffer, sizeof(pckt_buffer), 0, &in, &in_len) < 0) {
       fprintf(stderr, "Error: A problem occured receiving response.\n");
       return -1;
    }
  } else {
     //fprintf(stderr, "NORESPONSE\n");
     printf("NORESPONSE\n");
     return -2;
  }

  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "packet response output:\n");
    dump_packet(pckt_buffer, pckt_len);
  }
  ///////////////

  pckt_len = 0;

  // create received header struct
  header rec_header;

  /////* PARSE RESPONSE PACKET HEADER */////
  // copy header from packet buffer
  memcpy(&rec_header, &pckt_buffer, sizeof(header));

  // verify header fields and check for errors
  if (ntohs(rec_header.id) != HEADER_ID ||
    rec_header.qr != 1 ||
    rec_header.tc != 0 ||
    rec_header.ra != 1 ||
    rec_header.rd != 1) {
      fprintf(stderr, "Error: Response packet header is compromised.\n");
      return -1;
  }
  if (rec_header.rcode == 3) {
    //fprintf(stderr, "NOTFOUND\n");
    printf("NOTFOUND\n");
    return -1;
  }
  
  int answercnt = ntohs(rec_header.ancount); //number of answers
  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "answer count: %i\n", answercnt);
  }
  ///////////////
  
  pckt_len = sizeof(header);
  
  /////* PARSE RECEIVED QUESTION */////
  unsigned char* q_name = malloc(1 + strlen(name));
  /// parse received qname ///
  pckt_len += DNSToName(q_name, pckt_buffer, pckt_len);
  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "q_name after: %s\n", q_name);
  }
  ///////////////
  
  // check for qname consistency via compare to argv[2]
  if (strcmp((const char*)q_name, argv[2]) != 0) {
    fprintf(stderr, "Error: Received question name is compromised.\n");
    return -1;
  }
  
  //pckt_len += 2;
  
  question rec_question; // initialize question
  memcpy(&rec_question, &pckt_buffer[pckt_len], sizeof(question));
  
  //// DEBUG ////
  if (DEBUG == 1) {
    fprintf(stderr, "rec_question[pos]: %i\n", pckt_buffer[pckt_len]);
    fprintf(stderr, "rec_question.qtype: %i\n", ntohs(rec_question.qtype));
    fprintf(stderr, "rec_question.qclass: %i\n", ntohs(rec_question.qclass));
  }
  /////////////// 
  if (ntohs(rec_question.qclass) != 1) {
    fprintf(stderr, "Error: Received question is compromised.\n");
      return 1;
  }
  
  pckt_len += sizeof(question); // add onto packet length index
  
  /////* PARSE RECEIVED ANSWER */////
  unsigned char* q_name2 = malloc(1 + strlen(name));
  
  for(; answercnt > 0; answercnt--) { //for all answers
    memset(q_name2,0,sizeof(q_name2));
    
    /// parse received qname ///
    pckt_len += DNSToName(q_name2, pckt_buffer, pckt_len);
    //// DEBUG ////
    if (DEBUG == 1) {
      fprintf(stderr, "q_name2 after: %s\n", q_name);
    }
    ///////////////
  
    // check for qname consistency via compare to argv[2]
    if (strcmp((const char*)q_name, argv[2]) != 0) {
      fprintf(stderr, "Error: Received question name is compromised.\n");
      return -1;
    }
  
    answer rec_answer; // initialize answer
    /// parse rest received answer ///
    memcpy(&rec_answer, pckt_buffer+pckt_len, sizeof(answer));
    //// DEBUG ////
    if (DEBUG == 1) {
      fprintf(stderr, "rec_answer.type: %i\n", ntohs(rec_answer.type));
      fprintf(stderr, "rec_answer.class: %i\n", ntohs(rec_answer.class));
      fprintf(stderr, "rec_answer.ttl: %i\n", ntohs(rec_answer.ttl));
      fprintf(stderr, "rec_answer.rdlength: %i\n", ntohs(rec_answer.rdlength));
    }
    /////////////// 
  
    // verify class errors
    if (ntohs(rec_answer.class) != 1) {
      fprintf(stderr, "NOTFOUND\n");
      return -1; 
    }
    /// parse received data ///
    pckt_len += sizeof(answer) - 2;
    unsigned char* rd = malloc(156 * sizeof(char));
    // print IP address
    if (ntohs(rec_answer.type) == 1) {
      DNSToIP(rd, pckt_buffer, pckt_len);
      printf("IP\t%s", rd);
      pckt_len += ntohs(rec_answer.rdlength);
    }
    // print CNAME
    else if (ntohs(rec_answer.type) == 5) {
      pckt_len += DNSToName(rd, pckt_buffer, pckt_len);
      printf("CNAME\t%s", rd);
    }
    
    // auth or not based on information from header
    if (rec_header.aa == 1) {
      printf("\tauth\n");
    }
    else {
      printf("\tnonauth\n");
    }
    free(rd);
  }
    
  // free malloc'd variables
  free(ip_address);
  free(name);
  free(q_name);    
  free(q_name2);
  return 0;
}