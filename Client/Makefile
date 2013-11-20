#################################################################
##
## FILE:	Makefile
## AUTHOR:	Audrey Meinders and Marissa Wall
## PROJECT:	CS 3251 Project 2 - Professor Traynor
## DESCRIPTION: Compile Project 2 Client
##
################################################################# 

CC=gcc
LDFLAGS=-lssl -lcrypto

OS := $(shell uname -s)

# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: client
	
client: client.o parser.o networking.o
	$(CC) clientthread.o parser.o networking.o -o musicClient $(LDFLAGS)
	
client.o: clientthread.c parser.h
	$(CC) -c clientthread.c
	
parser.o: parser.c parser.h
	$(CC) -c parser.c

networking.o: networking.c networking.h
	$(CC) -c networking.c

clean:
	rm -f clientthread parser networking *.o