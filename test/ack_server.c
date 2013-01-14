#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <srtp.h>

/* ack_server - listens for packets from an SRTP client and sends valid ACK packets
 * in response. It does no protocol checking and so long as a well-formed packet
 * is received, it will acknowledge it.
 */

static void loop_for_connections(struct conn_state *conn) {
    conn->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (conn->fd < 0) {
        perror("socket");
        exit(5);
    }

    /* set up server connection */
    bzero(&conn->inp, sizeof(conn->inp));
    conn->inp.sin_family = AF_INET;
    conn->inp.sin_addr.s_addr = htonl(INADDR_ANY);
    conn->inp.sin_port = htons(conn->server_port);
    if (bind(conn->fd, (struct sockaddr *)&conn->inp, sizeof(conn->inp)) < 0) {
        perror("bind");
        exit(5);
    }

    printf("Listening for SRTP packets on UDP port %d\n", conn->server_port);
    printf("\n");

    while (1) {
        struct sockaddr_in client_addr;
        struct pktbuf buf;
        socklen_t len = sizeof(client_addr);

        /* copy the server state over to client */
        struct conn_state client_conn;
        memcpy(&client_conn, conn, sizeof(*conn));

        int bytes_read = recvfrom(conn->fd, (char *)&buf, sizeof(buf), 0, 
                (struct sockaddr *)&client_addr, &len);
        if (bytes_read < 0) {
            perror("recvfrom");
            continue;
        }
        char *src_ip = ip_from_sockaddr(&client_addr);
        printf("recvfrom: got %d bytes from <%s:%d>\n", bytes_read, src_ip,
                port_from_sockaddr(&client_addr));
        free(src_ip);

        if (bytes_read != sizeof(buf)) {
            fprintf(stderr, "recvfrom: dropping malformed packet\n");
            continue;
        }

        dump_packet_r(&buf);

        /* copy the source address into the connection so we can reply */
        memcpy(&client_conn.inp, &client_addr, sizeof(client_addr));
        
        /* send ACK packet back */
        struct pktbuf *a = create_ack_packet(buf.hdr.seqnbr);
        send_packet(&client_conn, a, false);
        dump_packet_s(a);
        free(a);
    }
}

int main(int argc, char **argv) {
    struct conn_state conn;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    int server_port = strtol(argv[1], NULL, 10);
    if (server_port <= 0) {
        fprintf(stderr, "Error: the port number given was invalid\n");
        return 1;
    }

    conn.server_ip = "0.0.0.0";
    conn.server_port = server_port;
    loop_for_connections(&conn);

    /* never get here */
    return 10;
}
