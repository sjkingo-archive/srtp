LIB=libsrtp.so.1

CC=gcc
CFLAGS=-Wall -Wextra -g -fPIC

$(LIB): client.o server.o
	$(CC) -shared -Wl,-soname,$(LIB) -o $(LIB) $^

.PHONY: clean
clean:
	rm -f $(LIB) *.o
