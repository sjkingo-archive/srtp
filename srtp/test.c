#include <stdio.h>

#include <srtp.h>

int main(void) {
    struct srtp_client_pcb cpcb;
    srtp_connect(&cpcb, "127.0.0.1", 5555);
    return 0;
}
