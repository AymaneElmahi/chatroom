CC = gcc 
DEBUG = -g
CFLAGS = -Wall -lpthread -c $(DEBUG)
LFLAGS = -Wall -lpthread $(DEBUG)

projet_rio = client-tcp.c server-tcp.c tools.c colors.h makefile message.h Rapport.pdf README.md tools.h

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

# génère un fichier projet_rio.tgz contenant les fichiers à déposer sur Moodle
projet: $(projet_rio)
	tar czvf projet_rio.tgz $(projet_rio)

clean:
	rm -rf *.o *~ client-tcp server-tcp
