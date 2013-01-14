LIB=libsrtp.so
CC=gcc
CFLAGS=-Wall -Wextra -g -fPIC -std=gnu99 -Wno-unused-parameter -I.

.PHONY: all
all: $(LIB)

$(LIB): client.o server.o net.o packet.o
	$(CC) -shared -Wl,-soname,$(LIB) -o $(LIB) $^

.PHONY: clean
clean:
	rm -f $(LIB) *.o
