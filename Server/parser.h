#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>                  /* for printf() and fprintf() */ 
#include <sys/socket.h>             /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>              /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>            /* for OpenSSL EVP digest libraries/SHA256 */
#include <openssl/sha.h>
#include <time.h>			 /* for time delay */
#include <sys/types.h>		 /* files */
#include <sys/stat.h>		 /* files */
#include <dirent.h>					/* for determining files in current directory */
#include <protobuf-c/protobuf-c.h>
#include <pthread.h>
#include <stdbool.h>

typedef enum {
    	LIST,
	DIFF,
	PULL,
	LEAVE
} method;

typedef struct {
	size_t size;
	int play_count;
	char *file_name;
} track;

void parseXML(char *file_name);
track *getOrderTrackList();
int getTrackCount();


#endif/*_PARSER_H_*/
