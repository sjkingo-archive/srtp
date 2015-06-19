srtp (Simple Reliable Transport Protocol)
=========================================

srtp is a library that implements a connection-oriented network transport
protocol running in user-space. It is somewhat based on:

http://itee.uq.edu.au/~coms4200/4.%20Assignments/Assignment\_1\_2012\_v1.0.pdf

Features
--------

srtp supports the following features:

* connection-oriented protocol (similar to TCP)
* message-based delivery of packets sent through the API
* in-order packet reconstruction (as it is not a stream protocol, this guarantees messages will be reconstructed in the correct order)
* unidirectional packet flow (data packets can only flow from client to server)
* reliable delivery of packets with retransmission using go-back-N
* uses UDP for underlying packet transfer and checksumming
* `test/nc0` - basic re-implementation of netcat using the srtp protocol

It does not implement any form of flow or congestion control. Due to the nature
of the protocol, only srtp clients and srtp servers can communicate with each
other.

Please note that the specification/protocol is not complete and may change at any time.

**This protocol is for research and testing only, it should not be used in production code!**

Client example
--------------

Full API documentation is available in `srtp/srtp.h`.

`client.c`:

```c
#include <stdio.h>
#include <string.h>
#include <srtp.h>

int main(void) {
    struct srtp_client_pcb cpcb;

    if (srtp_connect(&cpcb, "127.0.0.1", 5555) < 0) {
        perror("srtp_connect");
        return 1;
    }

    char *data = "Hello, world!\n";
    if (srtp_send(&cpcb, data, strlen(data)) < 0) {
        perror("srtp_send");
        return 2;
    }

    srtp_close(&cpcb);

    return 0;
}
```
