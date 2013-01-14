TESTS=test_connect ack_server

LIB_PATH=../srtp
CC=gcc
CFLAGS=-Wall -Wextra -g -std=gnu99 -Wno-unused-parameter -I$(LIB_PATH)
LDFLAGS=-Wl,-rpath $(LIB_PATH) -L$(LIB_PATH) -lsrtp

.PHONY: all
all: $(TESTS)

.PHONY: clean
clean:
	rm -f *.o $(TESTS)
