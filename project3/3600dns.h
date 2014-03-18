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
 /* ID: Identifier assigned by the program that generates any kind of query. */
  unsigned int id:16;
  /* QR: Specifies whether this message is a query (0), or a response (1). */
  unsigned int qr:1;
  /* OPCODE: Specifies kind of query in this message. */
  unsigned int opcode:4;
  /* Authoritative Answer: specifies that the responding name ip_address is an
   * authority for the domain name in question section */
  unsigned int aa:1;
  /* TrunCation: specifies that this message was truncated. */
  unsigned int tc:1;
  /* Recursion Desired: directs the name ip_address to pursue the query recursively. */
  unsigned int rd:1;
  /* Recursion Available: denotes whether recursive query support is available in 
   * the name ip_address. */
  unsigned int ra:1;
  /* Reserved for future use */
  unsigned int z:1;
  /* Response code: 4 bit field that is set as part of responses */
  unsigned int rcode:4;
  /* number of entries in the question section. */
  unsigned int qdcount:16;
  /* number of resource records in the answer section */
  unsigned int ancount:16;
  /* number of name ip_address resource records in the authority records section */
  unsigned int nscount:16;
  /* number of resource records in the additional records section. */
  unsigned int arcount:16;
} header;


#endif