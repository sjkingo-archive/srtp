#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <srtp.h>

struct pktbuf *create_packet(unsigned int type, unsigned int seqnbr, char *payload, int payload_len) {
    if (payload_len > SRTP_MAX_PAYLOAD_SIZE) {
        return NULL;
    }
    struct pktbuf *p = malloc(sizeof(*p));
    memset(p, 0, sizeof(*p));
    p->hdr.type = type;
    p->hdr.seqnbr = seqnbr;
    p->len = payload_len;
    if (payload != NULL) {
        memcpy(p->payload, payload, payload_len);
    }
    return p;
}

struct pktbuf *create_syn_packet(void) {
    return create_packet(SRTP_TYPE_SYN, 0, NULL, 0);
}

struct pktbuf *create_data_packet(void) {
    char *f = "this is a packet that needs to be longer than 64 characters so it properly wraps onto multiple lines maybe 3 lines would suffice!!!\n";
    return create_packet(SRTP_TYPE_DATA, 1, f, strlen(f));
}

struct pktbuf *create_ack_packet(int seqnbr) {
    return create_packet(SRTP_TYPE_ACK, seqnbr, NULL, 0);
}

char *str_packet_type(int type) {
    switch(type) {
        case SRTP_TYPE_DATA:
            return "SRTP_TYPE_DATA";
        case SRTP_TYPE_ACK:
            return "SRTP_TYPE_ACK";
        case SRTP_TYPE_SYN:
            return "SRTP_TYPE_SYN";
        case SRTP_TYPE_FIN:
            return "SRTP_TYPE_FIN";
        case SRTP_TYPE_RST:
            return "SRTP_TYPE_RST";
        default:
            return NULL;
    }
}

bool is_valid_type(int type) {
    return str_packet_type(type) != NULL ? true : false;
}

#define COLS_TO_PRINT 32
static void dump_packet(struct pktbuf *packet, char *prefix) {
    char *str_type = str_packet_type(packet->hdr.type);
    printf("%s Packet %s\tseqnbr=%d\n", prefix, str_type, packet->hdr.seqnbr);
    printf("%s payload len=%d\n", prefix, packet->len);
    if (packet->len > 0) {
        printf("%s ", prefix);
        for (int i = 0; i < packet->len; i++) {
            printf(" %02x ", packet->payload[i]);
            if ((i+1) % COLS_TO_PRINT == 0) {
                printf("\n%s ", prefix);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void dump_packet_r(struct pktbuf *packet) {
    dump_packet(packet, ">");
}

void dump_packet_s(struct pktbuf *packet) {
    dump_packet(packet, "<");
}
