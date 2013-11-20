#################################################################
##
## FILE:	Makefile
## AUTHOR:	Audrey Meinders and Marissa Wall
## PROJECT:	CS 3251 Project 2 - Professor Traynor
## DESCRIPTION: Compile Project 2 Server
##
#################################################################

CC=gcc
LDFLAGS=-lssl -lcrypto

OS := $(shell uname -s)

# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: server
	
server: serverthread.o networking.o
	$(CC) -lcrypto -lpthread serverthread.o networking.o -o musicServer $(LDFLAGS)
	
server.o: serverthread.c parser.h
	$(CC) -c serverthread.c
	
parser.o: parser.c parser.h
	$(CC) -c parser.c

networking.o: networking.c networking.h
	$(CC) -c networking.c

clean:
	rm -f serverthread parser *.o