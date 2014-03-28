/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600DNS_H__
#define __3600DNS_H__

//// ** DNS Header Structure ** ////
typedef struct {
  //// SEPARATED OUT BY BYTES ////
  unsigned int id:16; /* identifier assigned by the program that generates any kind of query. */
  ////
  unsigned int rd:1; /* directs the name ip_address to pursue the query recursively. */
  unsigned int tc:1; /* specifies that this message was truncated. */
  unsigned int aa:1; /* specifies that the responding name ip_address is an authority */
  unsigned int opcode:4; /* specifies kind of query in this message. */
  unsigned int qr:1; /* specifies whether this message is a query (0), or a response (1). */
  ////
  unsigned int rcode:4; /* 4 bit field that is set as part of responses */
  unsigned int z:3; /* Reserved for future use */
  unsigned int ra:1;  /* denotes whether recursive query support is available */  
  ////
  unsigned int qdcount:16; /* number of entries in the question section. */
  unsigned int ancount:16; /* number of resource records in the answer section */
  unsigned int nscount:16; /* number of name ip_address resource records in the authority records section */
  unsigned int arcount:16; /* number of resource records in the additional records section. */
} header;

typedef struct {
  int qtype:16;
  int qclass:16;
} question;

typedef struct {
  int type:16;
  int class:16;
  int ttl:32;
  int rdlength:16;
} answer;

#endif