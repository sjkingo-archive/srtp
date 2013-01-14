#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <srtp.h>

/* XXX caller needs to free this pointer */
struct conn_state *new_connection(char *server_ip, int server_port) {
    struct conn_state *s = malloc(sizeof(*s));
    memset(s, 0, sizeof(*s));

    s->server_ip = server_ip;
    s->server_port = server_port;

    s->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s->fd < 0) {
        perror("socket");
        free(s);
        return NULL;
    }

    memset((char *)&s->inp, 0, sizeof(s->inp));
    s->inp.sin_family = AF_INET;
    s->inp.sin_port = htons(server_port);
    if (inet_aton(server_ip, &s->inp.sin_addr) == 0) {
        perror("inet_aton");
        free(s);
        return NULL;
    }

    return s;
}

void block_for_ack(struct conn_state *conn, struct pktbuf *out, unsigned int expected_seq) {
    int bytes_read;
    socklen_t sock_len = sizeof(conn->inp);

    /* XXX max retries before giving up and resending?? */
    while (1) {
        bytes_read = recvfrom(conn->fd, (char *)out, sizeof(*out), 0, 
                (struct sockaddr *)&conn->inp, &sock_len);
        char *src_ip = ip_from_sockaddr(&conn->inp);
        printf("recvfrom: got %d bytes from <%s:%d>\n", bytes_read, 
                src_ip, port_from_sockaddr(&conn->inp));
        free(src_ip);

        /* check the packet is valid */
        if (bytes_read != sizeof(*out) || !is_valid_type(out->hdr.type)) {
            fprintf(stderr, "block_for_ack: dropping malformed packet\n");
            continue;
        }

        /* check that the packet is an ACK */
        if (out->hdr.type != SRTP_TYPE_ACK) {
            char *s = str_packet_type(out->hdr.type);
            fprintf(stderr, "block_for_ack: dropped unexpected %s packet\n", s);
            continue;
        }

        /* check that the packet has the correct sequence number */
        if (out->hdr.seqnbr != expected_seq) {
            fprintf(stderr, "block_for_ack: dropped unexpected SRTP_TYPE_ACK for seq=%d\n",
                    out->hdr.seqnbr);
            continue;
        }

        /* if we got here, then packet was what we expected */
        dump_packet_r(out);
        break;
    }
}

int send_packet(struct conn_state *conn, struct pktbuf *packet, bool wait_for_ack) {
    int bytes_to_send, bytes_sent;
    char *sc = str_conn(conn);
    struct pktbuf reply_pkt;

    /* send packet */
    bytes_to_send = sizeof(*packet);
    bytes_sent = sendto(conn->fd, (void *)packet, bytes_to_send, 0, 
            (struct sockaddr *)&conn->inp, sizeof(conn->inp));
    printf("sendto: sent %d bytes to %s\n", bytes_sent, sc);
    dump_packet_s(packet);
    free(sc);
    if (bytes_sent < 0) {
        perror("sendto");
    } else if (bytes_sent != bytes_to_send) {
        fprintf(stderr, "sendto: sent less bytes than expected\n");
        fprintf(stderr, "  given %d bytes\tsent %d bytes\n", bytes_to_send, bytes_sent);
    }

    if (wait_for_ack) {
        block_for_ack(conn, &reply_pkt, packet->hdr.seqnbr);
    }

    return bytes_sent;
}

/* XXX: caller must free */
char *str_conn(struct conn_state *c) {
    char *s = malloc(1024);
    snprintf(s, 1024, "<fd %d, %s:%d>", c->fd, c->server_ip, c->server_port);
    return s;
}

int port_from_sockaddr(struct sockaddr_in *addr) {
    return ntohs(addr->sin_port);
}

char *ip_from_sockaddr(struct sockaddr_in *addr) {
    socklen_t size = INET_ADDRSTRLEN;
    char *buf = malloc(size);
    if (inet_ntop(AF_INET, &addr->sin_addr, buf, size) == NULL) {
        free(buf);
        return NULL;
    }
    return buf;
}
