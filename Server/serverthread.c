/*///////////////////////////////////////////////////////////
*
* FILE:		serverthread.c
* AUTHOR:	Audrey Meinders and Marissa Wall
* PROJECT:	CS 3251 Project 2 - Professor Traynor
* DESCRIPTION:	Network Server Code
*
*////////////////////////////////////////////////////////////

/*Included libraries*/ 

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <openssl/evp.h>  /* for OpenSSL EVP digest libraries/SHA256 */
#include <pthread.h>	/*For threads*/
#include <sys/types.h>	/* files */
#include <sys/stat.h>	/* files */
#include <dirent.h>					/* for determining files in current directory */
#include "networking.h"
#include "parser.h"

void *ThreadMain(void *arg);		/* Main program of a thread */

pthread_mutex_t *mutex;

pthread_cond_t *cond;

int busy;


/* The main function */
int main(int argc, char *argv[])
{
	mutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if(!mutex)
		fatal_error("malloc failed\n");
	pthread_mutex_init(mutex,NULL);

	cond=(pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if(!cond)
		fatal_error("malloc failed\n");
	pthread_cond_init(cond,NULL);

	busy=0;

    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */

    /* Create new TCP Socket for incoming requests*/
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock < 0)
        fatal_error("Socket creation failed\n");
    int on = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* Construct local address structure*/
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(2001);
    
    /* Bind to local address structure */
    if (bind(serverSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        fatal_error("Bind failed");

    /* Listen for incoming connections */
    if (listen(serverSock, 5) < 0)
        fatal_error("Listen failed");
        

    /* Loop server forever*/
    while(1) 
    {

		/* Accept incoming connection */
		clntLen = sizeof(changeClntAddr);
		if ((clientSock = accept(serverSock, (struct sockaddr *) &changeClntAddr, &clntLen)) < 0)
			fatal_error("Accept failed\n");
		
		
		/* Create struct to pass in to thread */
		ThreadArgs *threadArgs = (ThreadArgs *) malloc(sizeof(ThreadArgs));
		if (threadArgs == NULL)
			fatal_error("malloc() failed\n");
		threadArgs->clientSock = clientSock;
		threadArgs->ip=inet_ntoa(changeClntAddr.sin_addr);
		
		/* Create client thread */
		pthread_t threadID;
		int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
		if (returnValue != 0)
			fatal_error("pthread_create failed\n");
	}
	free(mutex);
	free(cond);
}

void *ThreadMain(void *threadArgs)
{

	/* Tells the thread to deallocate resources when it returns*/
	pthread_detach(pthread_self());
	

	/* Get socket from the argument(struct) passed in */
	int clientSock = ((ThreadArgs *) threadArgs)->clientSock;


	char *ip =(char *)malloc(strlen(((ThreadArgs *) threadArgs)->ip));
	if(!ip)
		fatal_error("malloc failed\n");

	strcpy(ip,((ThreadArgs *) threadArgs)->ip);

	char *logStr="Log_File";

	while(1)
	{
		Header *header= receiveHeaderProto(clientSock);
		/*
		int method = header->method;
		int indexes=header->indexes;
		*/

		free(header);
		/*
		if(method==LIST)
		{
			fprintf(stderr,"LIST\n");
		
			if(!logFile(logStr,"LIST",ip,mutex,cond,&busy))
				fatal_error("List log file failed\n");
			
			if(!serverList(clientSock))
				fatal_error("server list failed\n");
		}
		else if(method==DIFF)
		{
			fprintf(stderr,"DIFF\n");
		
			if(!logFile(logStr,"DIFF",ip,mutex,cond,&busy))
				fatal_error("List log file failed\n");
			

			if(!serverDiff(clientSock))
				fatal_error("server diff failed\n"); 
		}
		else if(method==PULL)
		{
			fprintf(stderr,"PULL\n");
		
			if(!logFile(logStr,"PULL",ip,mutex,cond,&busy))
				fatal_error("List log file failed\n");
			
			if(!serverPull(clientSock,indexes))
				fatal_error("server pull failed\n"); 
		}
		else if(method==LEAVE)
		{
			fprintf(stderr,"LEAVE\n");
		
			if(!logFile(logStr,"LEAVE",ip,mutex,cond,&busy))
				fatal_error("List log file failed\n");
			

			if(!serverLeave(clientSock))
				fatal_error("server leave failed\n"); 
			break;
		}
		else
		{
			fatal_error("method does not exist\n");
		}
		*/

	}

	free(threadArgs);
	return (NULL);

}



int fatal_error(char *errormsg)
{
	printf("%s", errormsg);
	printf("\n");
	exit(0);
}
