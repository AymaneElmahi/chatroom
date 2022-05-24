CC = gcc 
DEBUG = -g
CFLAGS = -Wall -lpthread -c $(DEBUG)
LFLAGS = -Wall -lpthread $(DEBUG)

all: client server

client: client.o tools.o
	$(CC) $(LFLAGS) client.o tools.o -o client

server: server.o tools.o
	$(CC) $(LFLAGS) server.o tools.o -o server


client.o: client.c tools.h
	$(CC) $(CFLAGS) client.c

server.o: server.c tools.h
	$(CC) $(CFLAGS) server.c

tools.o: tools.h tools.c
	$(CC) $(CFLAGS) tools.c

clean:
	rm -rf *.o *~ client server
