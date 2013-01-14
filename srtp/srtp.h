#ifndef SRTP_H
#define SRTP_H

#define SRTP_MAX_PAYLOAD_SIZE    1024
#define SRTP_MAX_SEND_BUF_SIZE   8

/*** SRTP packet types ***/
#define SRTP_TYPE_DATA  1       /* Data packet */
#define SRTP_TYPE_ACK   2       /* Acknowledgement packet*/
#define SRTP_TYPE_SYN   3       /* SYN packet, initiates connection setup */ 
#define SRTP_TYPE_FIN   4       /* FIN packet, initiates closing of connection */
#define SRTP_TYPE_RST   5       /* Reset packet, resets connection */

/* client and server states */
#define SRTP_STATE_ESTABLISHED  1
#define SRTP_STATE_CLOSED       2

/* client only states */
#define SRTP_STATE_SYN_SENT     3
#define SRTP_STATE_FIN_WAIT     4  

/* server only states */
#define SRTP_STATE_LISTEN       5
#define SRTP_STATE_TIME_WAIT    6

/* for uint32_t type */
#include <stdint.h>

/* common includes */
#include <net.h>
#include <packet.h>

/*** SRTP Header ***/
struct srtp_hdr {
    uint32_t type;
    uint32_t seqnbr;
};

/*** SRTP Packet **/
struct pktbuf {
    struct srtp_hdr hdr;                
    int len; /* size of payload in bytes */
    char payload[SRTP_MAX_PAYLOAD_SIZE];
};

/* client PCB */
struct srtp_client_pcb {
    struct conn_state *conn; /* the connection */
    int state;  /* connection state: CLOSED, ESTABLISHED, SYN_SENT or FIN_WAIT */
    struct pktbuf send_buf[SRTP_MAX_SEND_BUF_SIZE];  /* you need to define SRTP_MAX_SEND_BUF_SIZE */
};

/* server PCB */
struct srtp_server_pcb {
    struct conn_state *conn; /* the connection */
    int state;  /* connection state: CLOSED, ESTABLISHED, LISTEN, FIN_WAIT */
};


/*
    Function:    srtp_connect
    Purpose:     Called by the client to establish a connection to a server. 
                 Performs 2-way connection establishment handshake. 
                 In case of success, the client PCP structure (cpcb) is initialised.
    Parameters: 
        cpcb    : client PCB 
        dst_addr: string that represents server IP address, e.g. "192.168.2.33" 
        dst_port: server port number
    

    Returns:     0 on success, -1 on failure
*/
int srtp_connect(struct srtp_client_pcb *cpcb, char *dst_addr, int dst_port);




/*
    Function:    srtp_close
    Purpose:     Called by the client to terminate a connection. 
                 Performs 2-way connection termination handshake. 
                 The client PCP structure (cpcb) is updated accordingly, i.e. state -> Closed.

    Parameters: 
        cpcb    : client PCB 
    
    Returns:     0 on success, -1 on failure
*/
int srtp_close(struct srtp_client_pcb *cpcb);



/*
    Function:    srtp_send
    Purpose:    Called by the client application to send a data packet to the server. 
                The maximum number of bytes the function accepts is SRTP_MAX_PAYLOAD_SIZE.

                Since our protocol is implemented at the application layer, we cannot rely
                on the kernel to do all the asyncronoys ARQ processing such as receiving and
                processing of ACKs, managing time-outs, retransmitting data packets etc.
                
                The function therefore needs to provide functionality such as the following:
                - Receiving and processing of ACKs
                - If there is space in the send buffer, put data packet into send buffer,
                  send it over the network, and start a corresponding timer.
                - If the send buffer is already full, the function should sleep for a short
                  amount of time, and then see if there are any new ACKs that would result in
                  freed up space in the send buffer.
                - The function also needs to check of there are any retransmission timeouts, and
                  needs to do the necessary retransmissions, and update the timers.  
                - ...

    Parameters: 
        cpcb    : client PCB  
        data    : pointer to buffer with data to send
        len        : number of bytes to send
    
    Returns:     0 on success, -1 on failure
*/
int srtp_send(struct srtp_client_pcb *cpcb, void *data, int len);



/*
    Function:    srtp_client_reset
    Purpose:     Called by the client to reset a connection.
                 Independent of the state of the connection (except for the CLOSED state), 
                 this function sends a RST packet to the server, and resets the connection to the CLOSED state.
                 
    Parameters: 
        cpcb    : client PCB 
    
    Returns:     0 on success, -1 on failure
*/
int srtp_client_reset(struct srtp_client_pcb *cpcb);




/******* Server Functions *******/

/*
    Function:   srtp_listen
    Purpose:    The function opens a UDP socket on the specified port and initialises the server PCB accordingly.
                 
    Parameters: 
        spcb    : server PCB 
        port    : port number on which server is listening

    Returns:     0 on success, -1 on failure
*/
int srtp_listen(struct srtp_server_pcb *spcb, int port);


/*
    Function:   srtp_accept
    Purpose:    The function blocks and waits for SYN packets. It performs the 2-way connection establishment handshake.
                The function can only be called if the connection is in state LISTEN. The function updates the server PCB
                according to the outcome of the 2-way handshake.
                 
    Parameters: 
        spcb    : server PCB  
    
    Returns:     0 on success, -1 on failure
*/
int srtp_accept(struct srtp_server_pcb *spcb);



/*
    Function:   srtp_server_reset
    Purpose:    Independent of the state of the connection (except for the CLOSED state),
    this function sends a RST packet to the client, and returns to the CLOSED state.

                 
    Parameters: 
        spcb    : server PCB  
    
    Returns:     0 on success, -1 on failure
*/
int srtp_server_reset(struct srtp_server_pcb *spcb);
 


/*
    Function:   srtp_receiver_loop
    Purpose:    This function handles all the ARQ processing on the server side. 
                The function implements a loop in which data packets are received from the client,
                and the corresponding ACKs are sent etc. To simulate passing data to the application
                the function simply writes data to the file with name 'outfile'. 

                The function also needs to handle the 2-way connection termination handshake.

                The function should react to any protocol error, i.e. unexpected behaviour of the client
                with calling srtp_server_reset() and aborting.
                 
    Parameters: 
        spcb    : server PCB  
        outfile    : name of file we write data to
    
    Returns:     0 on success, -1 on failure
*/
int srtp_receive_loop(struct srtp_server_pcb *spcb, char *outfile);

#endif
