#ifndef SRTP_UTIL_H
#define SRTP_UTIL_H

#include "srtp.h"

/* generic function to create a new packet - caller must free returned pointer */
struct pktbuf *create_packet(unsigned int type, unsigned int seqnbr, char *payload, int payload_len);

/* functions to create various types of packets - caller must free returned pointer */
struct pktbuf *create_syn_packet(void);
struct pktbuf *create_data_packet(void);
struct pktbuf *create_ack_packet(int seqnbr);

/* return a string representation of a packet's type - caller must free returned pointer */
char *str_packet_type(int type);

/* dump a packet's contents to stdout. _r is for packets recv'd, _s is for packets sent */
void dump_packet_r(struct pktbuf *packet);
void dump_packet_s(struct pktbuf *packet);

/* returns true if a packet is of a valid type */
#include <stdbool.h>
bool is_valid_type(int type);

#endif
