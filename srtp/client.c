#include <stdio.h>
#include <stdlib.h>

#include <srtp.h>

int srtp_connect(struct srtp_client_pcb *cpcb, char *dst_addr, int dst_port) {
    cpcb->conn = new_connection(dst_addr, dst_port);
    if (cpcb->conn == NULL) {
        /* this will only fail if there is some low-level socket error -
         * nothing has been sent to the server yet */
        return -1;
    }

    /* send a SYN packet (handshake). if this returns then a valid response was recv'd */
    struct pktbuf *p = create_syn_packet();
    send_packet(cpcb->conn, p, true);
    free(p);

    printf("Two-way handshake complete\n");
    printf("Connection to %s:%d successful\n", dst_addr, dst_port);

    return 0;
}

int srtp_close(struct srtp_client_pcb *cpcb) {
    return -1;
}

int srtp_send(struct srtp_client_pcb *cpcb, void *data, int len) {
    return -1;
}

int srtp_client_reset(struct srtp_client_pcb *cpcb) {
    return -1;
}
