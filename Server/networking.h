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
#include <protobuf-c/protobuf-c.h>
#include <pthread.h>
#include <stdbool.h>
#include "structs.pb-c.h"

 
/* Constants */
#define RCVBUFSIZE 4096             /* The receive buffer size */
#define SNDBUFSIZE 4096             /* The send buffer size */
#define TITLELEN 150
#define LENGTH_PREFIX_SIZE sizeof(uint32_t)

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
	bool capLimitSkip;	/* Flag that indicates if a song was skipped in a PULL due to the CAP */
} song;

/* Struct for arguments that will be passed to client */
typedef struct {
	int clientSock;	/*Socket descripter for client*/
	char *ip;
	int32_t cap;
}ThreadArgs;

int clientList(int sock);
int serverList(int sock);
int clientDiff(int sock);
int serverDiff(int sock);
int clientPull(int sock);
int serverPull(int sock, Header *header, ThreadArgs *settings);
int clientLeave(int sock);
int serverLeave(int sock);
int serverCap(int sock);

void orderSongs(Song **diffSongs, int diffSongCount);
int fileLen(FILE *file);

int logFile(char *fileName, char *method, char *ip, pthread_mutex_t *mutex, pthread_cond_t *cond, int *busy);

song *compareSongDir(song *server, int serverLen, song *client, int clientLen, int *lenOfNewArr);
Song **compareSongDirProto(Song **server, int serverLen, Song **client, int clientLen, int *lenOfNewArr, ThreadArgs *settings);
int numSongsInDir();
song *createSongArray(int numSongs);
int calculateChecksum(FILE *file,song *s);
int calculateChecksumProto(FILE *file, Song *s);
int sendSongArray(song *songs,int num,int sock);
song *recvSongArray(int length,int sock);

int sendHeader(int method,int numBytesToSend,int indexes, int sock);
int sendHeaderProto(int method, Song **songs, int numSongs, int sock);
header *receiveHeader(int sock);
Header *receiveHeaderProto(int sock);
Song **createSongArrayProto(int numSongs);

FILE * receiveFile(FILE *file, int numBytesToWrite, int sock);
int sendFile(FILE *file, int sock);

int fatal_error(char *errormsg);

static char *xstrdup (const char *str) {
  if (str == NULL)
    return NULL;
  return strcpy (malloc (strlen (str) + 1), str);
}

#endif

