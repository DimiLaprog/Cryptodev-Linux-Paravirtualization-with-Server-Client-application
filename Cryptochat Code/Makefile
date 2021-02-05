###################################################
#
# Makefile
# Simple TCP/IP communication using sockets
#
# Vangelis Koukis <vkoukis@cslab.ece.ntua.gr>
#
###################################################

CC = gcc

CFLAGS = -Wall
CFLAGS += -g
# CFLAGS += -O2 -fomit-frame-pointer -finline-functions

LIBS = chat.c

BINS = socket-server socket-client

all: $(BINS)

socket-server: socket-server.c chat.c socket-common.h chat.h
	$(CC) $(CFLAGS) $(LIBS) $< -o $@

socket-client: socket-client.c chat.c socket-common.h chat.h
	$(CC) $(CFLAGS) $(LIBS) $< -o $@ 

clean:
	rm -f *.o *~ $(BINS)
