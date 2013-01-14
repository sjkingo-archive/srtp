#ifndef SRTP_NET_H
#define SRTP_NET_H

#include <stdbool.h>
#include <netinet/in.h>
#include "srtp.h"
struct pktbuf;

struct conn_state {
    int fd;
    struct sockaddr_in inp;
    char *server_ip;
    int server_port;
};

struct conn_state *new_connection(char *server_ip, int server_port);

int send_packet(struct conn_state *conn, struct pktbuf *packet, bool wait_for_ack);

char *str_conn(struct conn_state *c);

/* helper functions to return the readable representation of sockaddr ip/port.
 * Note caller must free the pointer returned from ip_from_sockaddr()
 */
int port_from_sockaddr(struct sockaddr_in *addr);
char *ip_from_sockaddr(struct sockaddr_in *addr);

#endif
