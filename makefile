CC = gcc 
DEBUG = -g
CFLAGS = -Wall -lpthread -c $(DEBUG)
LFLAGS = -Wall -lpthread $(DEBUG)

all: client-tcp server-tcp

client-tcp: client-tcp.o tools.o
	$(CC) $(LFLAGS) client-tcp.o tools.o -o client-tcp

server-tcp: server-tcp.o tools.o
	$(CC) $(LFLAGS) server-tcp.o tools.o -o server-tcp


client-tcp.o: client-tcp.c tools.h
	$(CC) $(CFLAGS) client-tcp.c

server-tcp.o: server-tcp.c tools.h
	$(CC) $(CFLAGS) server-tcp.c

tools.o: tools.h tools.c
	$(CC) $(CFLAGS) tools.c

clean:
	rm -rf *.o *~ client-tcp server-tcp
