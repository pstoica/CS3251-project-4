# ifndef NETWORKING_H
# define NETWORKING_H

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
#include "song.pb-c.h"
#include "header.pb-c.h"

 
/* Constants */
#define RCVBUFSIZE 700000             /* The receive buffer size */
#define SNDBUFSIZE 700000             /* The send buffer size */
#define TITLELEN 150


/* struct that contains info about the method being invoked and the information to follow */
typedef struct{
	int method;
	int length;
	int indexes;
} header;

/* struct to contain info about a song */
typedef struct {
	char title[TITLELEN]; //length subject to change
	unsigned char checksum[SHA256_DIGEST_LENGTH];		/* computed checksum of the song */
	int lenOfSong;	/* length in bytes of the actual music file */	
} song;

/* Struct for arguments that will be passed to client */
typedef struct {
	int clientSock;	/*Socket descripter for client*/
	char *ip;
}ThreadArgs;

int clientList(int sock);
int serverList(int sock);
int clientDiff(int sock);
int serverDiff(int sock);
int clientPull(int sock);
int serverPull(int sock,int indexes);
int clientLeave(int sock);
int serverLeave(int sock);

int fileLen(FILE *file);

int logFile(char *fileName, char *method, char *ip, pthread_mutex_t *mutex, pthread_cond_t *cond, int *busy);

song *compareSongDir(song *server, int serverLen, song *client, int clientLen, int *lenOfNewArr);
int numSongsInDir();
song *createSongArray(int numSongs);
int calculateChecksum(FILE *file,song *s);
int sendSongArray(song *songs,int num,int sock);
song *recvSongArray(int length,int sock);

int sendHeader(int method,int numBytesToSend,int indexes, int sock);
header * receiveHeader(int sock);

FILE * receiveFile(FILE *file, int numBytesToWrite, int sock);
int sendFile(FILE *file, int sock);


#endif

