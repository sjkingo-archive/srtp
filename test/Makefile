LIB_PATH=../srtp
CC=gcc
CFLAGS=-Wall -Wextra -g -std=gnu99 -Wno-unused-parameter -I$(LIB_PATH)

.PHONY: all
all: ack_server

.PHONY: clean
clean:
	rm -f *.o ack_server

ack_server: ack_server.o
	$(CC) -o $@ -Wl,-rpath $(LIB_PATH) -L$(LIB_PATH) -lsrtp $^
