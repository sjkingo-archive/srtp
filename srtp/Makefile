LIB=libsrtp.so
LIB_PATH=.
CC=gcc
CFLAGS=-Wall -Wextra -g -fPIC -std=gnu99 -Wno-unused-parameter -I$(LIB_PATH)

.PHONY: all
all: $(LIB) test

$(LIB): client.o server.o net.o packet.o
	$(CC) -shared -Wl,-soname,$(LIB) -o $(LIB) $^

.PHONY: clean
clean:
	rm -f $(LIB) *.o test

test: test.o
	$(CC) -o $@ -Wl,-rpath $(LIB_PATH) -L$(LIB_PATH) -lsrtp $^
