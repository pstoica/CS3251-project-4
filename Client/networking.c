#include "networking.h"


/* client function that performs the list method */
int clientList(int sock)
{
	if(!sendHeader(0,0,0, sock))
		fatal_error("send header has failed\n");
	
	header *rcvHead= receiveHeader(sock);
	if(!rcvHead)
		fatal_error("error receiving rcv header\n");

	if(!rcvHead->indexes&&!rcvHead->length)
	{
		free(rcvHead);
		fprintf(stderr,"There are no songs on the server.\n\n");
		return 1;
	}
	
	
	song *songs=recvSongArray(rcvHead->indexes,sock);
	if(!songs)
		fatal_error("error receiving song array\n");
	
	fprintf(stderr,"Songs on the server are:\n");
	int i;
	for(i=0;i<rcvHead->indexes;i++)
	{
		fprintf(stderr,"%s\n",songs[i].title);
	}

	fprintf(stderr,"\n");

	free((void*)rcvHead);
	free((void*)songs);
	return 1;
}


/* sever function that performs the list method */
int serverList(int sock)  
{
	int numSongs=numSongsInDir();
	if(!numSongs)
	{
		if(!sendHeader(0,0,0,sock))
			fatal_error("send header has failed\n");
		return 1;
	}
	if(!sendHeader(0,numSongsInDir()*sizeof(song), numSongs,sock))
		fatal_error("send header has failed\n");

	song *songs=createSongArray(numSongs);
	if(!songs)
		fatal_error("creating song array failed\n");
		
	if(sendSongArray(songs,numSongs,sock)!=sizeof(song)*numSongs)
		fatal_error("sending song array failed\n"); 	
	
	free((void*)songs);
	return 1;
}


/* client function that performs the diff method */
int clientDiff(int sock)
{
	if(!sendHeader(1,0,0, sock))
		fatal_error("send header has failed\n");
		
	header *rcvHead= receiveHeader(sock);
	if(!rcvHead)
		fatal_error("error receiving rcv header\n");
	
	if(!rcvHead->indexes&&!rcvHead->length)
	{
		free(rcvHead);
		fprintf(stderr,"There are no songs on the server.\n\n");
		return 1;

	}

	song *rcvSongs=recvSongArray(rcvHead->indexes,sock);
	if(!rcvSongs)
		fatal_error("error receiving song array\n");
	
	int i;

	song *clientSongs=0;
	int numSongs=numSongsInDir();

	if(numSongs<0)
		fatal_error("number of songs calculation failed\n");
	else if(numSongs>0)
	{
		clientSongs=createSongArray(numSongs);

		int diffLen=0;		

		song *diffSongs = compareSongDir(rcvSongs, rcvHead->indexes, clientSongs, numSongs,&diffLen);
		if(!diffLen)
		{
			fprintf(stderr,"You have all the songs that are on the server.\n\n");
			free(diffSongs);
			free(clientSongs);
			free(rcvHead);
			free(rcvSongs);
			return 1;
		}

		fprintf(stderr,"Songs on the server that you do not have are:\n");
		for(i=0;i<diffLen;i++)
		{
			fprintf(stderr,"%s\n",diffSongs[i].title);
		}
		fprintf(stderr,"\n");
		free(diffSongs);
		free(clientSongs);
	}
	else
	{
		fprintf(stderr,"Songs on the server that you do not have are:\n");
		for(i=0;i<rcvHead->indexes;i++)
		{
			fprintf(stderr,"%s\n",rcvSongs[i].title);
		}
		fprintf(stderr,"\n");
	}
	free(rcvHead);
	free(rcvSongs);
	return 1;
}


/* sever function that performs the diff method */
int serverDiff(int sock)
{
	int numSongs=numSongsInDir();

	if(!numSongs)
	{
		if(!sendHeader(1,0,0,sock))
			fatal_error("send header has failed\n");
		return 1;
	}

	if(!sendHeader(1,numSongsInDir()*sizeof(song), numSongs, sock))
		fatal_error("send header has failed\n");
		
	song *songs=createSongArray(numSongs);
	if(!songs)
		fatal_error("creating song array failed\n");
		
	if(sendSongArray(songs,numSongs,sock)!=sizeof(song)*numSongs)
		fatal_error("sending song array failed\n"); 	
	
	free((void*)songs);
	return 1;
}

/* client function that performs the pull method */
int clientPull(int sock)
{
	int numSongs=numSongsInDir();

	if(numSongs)
	{

		if(!sendHeader(2, numSongs*sizeof(song), numSongs, sock))
			fatal_error("send header has failed\n");

		song *songs=createSongArray(numSongs);
		if(!songs)
			fatal_error("creating song array failed\n");

		if(sendSongArray(songs,numSongs,sock)!=sizeof(song)*numSongs)
			fatal_error("sending song array failed\n"); 

		free(songs);
	}
	else
	{
		if(!sendHeader(2, 0, 0, sock))
			fatal_error("send header has failed\n");
	}
		
	header *rcvHead= receiveHeader(sock);
	if(!rcvHead)
		fatal_error("error receiving rcv header\n");
		
	if(!rcvHead->indexes)
	{
		fprintf(stderr,"You have all the songs that are on the server.\n\n");
	}	
	else
	{
		song *rcvSongs=recvSongArray(rcvHead->indexes,sock);
		if(!rcvSongs)
			fatal_error("error receiving song array\n");
			
		fprintf(stderr, "The following songs have been added to your directory:\n");
		int i;
		for(i=0;i<rcvHead->indexes;i++)
		{
			FILE *file = fopen(rcvSongs[i].title,"w+");
			if(!receiveFile(file, rcvSongs[i].lenOfSong, sock))
				fatal_error("receive file failed\n");
			fclose(file);
			fprintf(stderr, "%s\n",rcvSongs[i].title);
		}
		fprintf(stderr,"\n");
		free(rcvSongs);
	}
	free(rcvHead);
	return 1;
		
}
/* server function that performs the pull method */
int serverPull(int sock,int indexes)
{	
	int numSongs=numSongsInDir();
	if(!indexes)//if the client has no songs
	{
		if(!numSongs) //if the server has no songs
		{
			if(!sendHeader(2,0, 0, sock))
				fatal_error("send header has failed\n");
		}
		else //if the server has songs
		{
			song *serverSongs=createSongArray(numSongs);
			
			if(!sendHeader(2,numSongs*sizeof(song), numSongs, sock))
				fatal_error("send header has failed\n");
			
			if(sendSongArray(serverSongs,numSongs,sock)!=sizeof(song)*numSongs)
				fatal_error("sending song array failed\n"); 
				
			int i;
			for(i=0;i<numSongs;i++)
			{
				FILE *file = fopen(serverSongs[i].title,"r+");
				if(!sendFile(file, sock))
					fatal_error("receive file failed\n");
			}
			free(serverSongs);
		}
	}
	else //the client has songs
	{
		if(!numSongs) //if the server has no songs
		{
			song *rcvSongs=recvSongArray(indexes,sock);
			if(!rcvSongs)
				fatal_error("error receiving song array\n"); 
				
			if(!sendHeader(2,0, 0, sock))
				fatal_error("send header has failed\n");
				
			free(rcvSongs);
		}
		else //the server has songs
		{
			song *rcvSongs=recvSongArray(indexes,sock);
			if(!rcvSongs)
				fatal_error("error receiving song array\n"); 
		
			song *serverSongs=createSongArray(numSongs);

			int diffLen=0;		

			song *diffSongs = compareSongDir(serverSongs, numSongs,rcvSongs,indexes, &diffLen);

			if(!diffLen) // they have the same songs
			{
				if(!sendHeader(2,0, 0, sock))
					fatal_error("send header has failed\n");
			}
			else //they do not have the same songs
			{
				if(!sendHeader(2,diffLen*sizeof(song), diffLen, sock))
					fatal_error("send header has failed\n");
			
				if(sendSongArray(diffSongs,diffLen,sock)!=sizeof(song)*diffLen)
					fatal_error("sending song array failed\n"); 
				
				int i;
				for(i=0;i<diffLen;i++)
				{
					FILE *file = fopen(diffSongs[i].title,"r+");
					if(!sendFile(file, sock))
						fatal_error("receive file failed\n");
					
				}
			}
			free(rcvSongs);
			free(diffSongs);
			free(serverSongs);
		}
	}
	
	return 1;	
}	
/* client function that performs the leave method */
int clientLeave(int sock)
{
	if(!sendHeader(3,0,0, sock))
		fatal_error("send header has failed\n");
		
	header *rcvHead= receiveHeader(sock);
	if(!rcvHead)
		fatal_error("error receiving rcv header\n");
		
	if(rcvHead->method!=3)
		fatal_error("leave did not work\n");
	free(rcvHead);
	return 1;
}

/* sever function that performs the leave method */
int serverLeave(int sock)
{
	if(!sendHeader(3,0,0, sock))
		fatal_error("send header has failed\n");
	
	return 1;
}

/* returns length of file */
int fileLen(FILE *file)
{
	if(!file) 
		fatal_error("not a file\n");

	/* note current position in the file */
	int currPosition=ftell(file);
	
	/* go to the end of the file */
	if(fseek(file, 0, SEEK_END))
		fatal_error("failed to find end of file\n");

	/* get the current position (at the end of the file) */
	int len = ftell(file);		
	
	/* make the current position the original location */
	if(fseek(file, currPosition, SEEK_SET))
		fatal_error("failed to return back to previous position\n");

	return len;

}
/* creates an array of songs that the server has but the client does not have, server and client arrays are not freed */
song *compareSongDir(song *server, int serverLen, song *client, int clientLen, int *lenOfNewArr)
{
	int maxLen=(serverLen>clientLen)?serverLen:clientLen;
	song* maxList=malloc(maxLen*sizeof(song));
	if(!maxList)
		fatal_error("malloc failed to create space for a song array\n");
	
	int s = 0;
	int c = 0;	
	char b = 0; /* 0 represents none are equal */
	int numDiff = 0;

	while(serverLen>s)
	{
		c=0;
		b=0;
		while(clientLen>c)
		{
			int i;
			
			
			if(!memcmp(server[s].checksum,client[c].checksum,SHA256_DIGEST_LENGTH))
			{
				b=1;
			}
			c++;
		}
		if(!b)
		{
			memcpy(&(maxList[numDiff]),&(server[s]),sizeof(song));
			numDiff++;
		}
		s++;
	}

	(*lenOfNewArr)=numDiff;
	
	if(!lenOfNewArr)
	{
		free(maxList);
		return 0;
	}

	if(maxLen!=serverLen)
	{
		song *minList = (song *)malloc(numDiff*sizeof(song));
		if(!minList)
			fatal_error("malloc failed to create space for a song array\n");
		memcpy(minList,maxList,numDiff*sizeof(song));

		free(maxList);
		return minList;
	}
	else
	{
		return maxList;
	}
}

/* returns the number of songs in the current working directory */
int numSongsInDir()
{
	int numOfSongs=0;
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			char *name=dir->d_name;
			int len =strlen(name);

			if(len>4&&len<TITLELEN&&name[len-4]=='.'&&name[len-3]=='m'&&name[len-2]=='p'&&name[len-1]=='3')
			{
				numOfSongs++;
			}
		}

		closedir(d);
	}

	return numOfSongs;
}

/* calculates check sum of the file and places it in the song struct, file pointer is set back to the beginning of the file */
int calculateChecksum(FILE *file,song *s)
{
	unsigned char digest[SHA256_DIGEST_LENGTH];
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	int md_len;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("SHA256");
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, NULL);
	
	int bufferSize = 500000;

	char *buff = malloc(bufferSize);
	if(!buff)
		fatal_error("malloc failed\n");

	int numBytesToRead = fileLen(file);
	int currBytesRead =0;
	int bytesRead = 0;

	while(numBytesToRead>bytesRead)
	{

		/* checks if bytes to be read in this iteration are greater than buffer */
		int currNumBytesToRead=(numBytesToRead-bytesRead>bufferSize)?bufferSize:numBytesToRead-bytesRead;

		/* read bytes */
		currBytesRead=fread(buff,1,currNumBytesToRead,file);
		if(currBytesRead<= 0)
			fatal_error("read failed\n"); 
		

		/* update md5 calculation */
		EVP_DigestUpdate(mdctx, buff, currNumBytesToRead);

		/* calculate the total number of bytes read thus far */
		bytesRead += currBytesRead;
	}

	EVP_DigestFinal_ex(mdctx, digest, &md_len);
	EVP_MD_CTX_destroy(mdctx);

	memcpy(s->checksum,digest,md_len);

	free(buff);
	
	if(fseek(file, 0, SEEK_SET))
		fatal_error("failed to return back to the beginning of the file\n");
	
	return 1;
}



/* crawls through directory and creates an array of song structs */
song *createSongArray(int numSongs)
{	
	if(!numSongs)
		return 0;

	song *songBuf= (song *)malloc(sizeof(song)*numSongs);
	if(!songBuf)
		fatal_error("malloc memory for songBuf failed\n");
	
	memset(songBuf,0,sizeof(song)*numSongs);
	int i =0;
	
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			char *name=dir->d_name;
			int len =strlen(name);
			if(len>4&&len<TITLELEN&&name[len-4]=='.'&&name[len-3]=='m'&&name[len-2]=='p'&&name[len-1]=='3')
			{
				memcpy(&(songBuf[i].title),(name),len);
				
				FILE *file = fopen(name,"r+"); 

				songBuf[i].lenOfSong=fileLen(file);
				
				if(!calculateChecksum(file,&(songBuf[i])))
					fatal_error("checksum failed to calculate\n");
	
				fclose(file);

			
				i++;
			}
		}

		closedir(d);
	}
	return songBuf;
}

/* sends song array DOES NOT FREE SONG ARRAY!*/
int sendSongArray(song *songs,int num,int sock)
{
	if(!num)
		fatal_error("num of song array is 0\n");
	
	if(!songs)
		fatal_error("song array is null\n");
		
	if(!sock)
		fatal_error("send song array failed because sock is null\n");
		
	int totalBytesSent = 0;
	int numBytesSent = 0;
	
	/* while there are still bytes left to send */
	while(totalBytesSent <sizeof(song)*num)
	{
		/* send header */
		numBytesSent = send(sock, &(songs[totalBytesSent]), sizeof(song)*num-totalBytesSent, 0);
		if(numBytesSent<0)
			fatal_error("header send failed");
		totalBytesSent+=numBytesSent;
	} 

	return totalBytesSent;
}

/* receives song array and returns it */
song *recvSongArray(int length,int sock)
{
	if(!length)
		fatal_error("length of song array is 0\n");
	
	if(!sock)
		fatal_error("receive header failed because sock is null \n");
		
	char *rcvSongBuf;                   /* Receive Head Buffer */
	
	/* malloc space for head buffer */

	rcvSongBuf= (char *)malloc(sizeof(song)*length);
	if(!rcvSongBuf)
		fatal_error("malloc memory for rcvSongBuf failed\n");


	memset(rcvSongBuf, 0, sizeof(song)*length);
		
	int totalBytesRcvd = 0;
	int numBytesRcvd = 0;

	/* while total bytes received is less than what is expected */
	while(totalBytesRcvd<sizeof(song)*length)
	{
		/* receive header */
		numBytesRcvd = recv(sock, &(rcvSongBuf[totalBytesRcvd]), sizeof(song)*length-totalBytesRcvd, 0);
		if (numBytesRcvd < 0)
	          	fatal_error("header recv failed\n");
		totalBytesRcvd += numBytesRcvd;
	}
	
	return (song *)rcvSongBuf;
}

/* pass in header struct, send struct as is, at the end of the method the header struct is freed */
int sendHeader(int method,int numBytesToSend,int index, int sock)
{
	char *head;                   /* Send HeadBuffer */
	head= (char *)malloc(sizeof(header));
	if(!head)
		fatal_error("malloc memory for head failed\n");

	memset(head, 0, sizeof(header));
	
	((header *) head)->method=method;
	((header *) head)->length=numBytesToSend;
	((header *) head)->indexes=index;

	if(!head)
		fatal_error("header to be sent is null\n");
		
	if(!sock)
		fatal_error("send header failed because sock is null \n");
		
	int totalBytesSent = 0;
	int numBytesSent = 0;
	
	/* while there are still bytes left to send */
	while(totalBytesSent <sizeof(header))
	{
		/* send header */
		numBytesSent = send(sock, &(head[totalBytesSent]), sizeof(header)-totalBytesSent, 0);
		if(numBytesSent<0)
			fatal_error("header send failed");
		totalBytesSent+=numBytesSent;
	} 
	
	free(head);
	return totalBytesSent;
}

/* pass in socket that is expecting a header, receive header * to the recceived header*/
header * receiveHeader(int sock)
{
	
	if(!sock)
		fatal_error("receive header failed because sock is null \n");
		
	char *rcvHeadBuf;                   /* Receive Head Buffer */
	
	/* malloc space for head buffer */
	rcvHeadBuf= (char *)malloc(sizeof(header));
	if(!rcvHeadBuf)
		fatal_error("malloc memory for rcvHeadBuf failed\n");


	memset(rcvHeadBuf, 0, sizeof(header));
		
	int totalBytesRcvd = 0;
	int numBytesRcvd = 0;

	/* while total bytes received is less than what is expected */
	while(totalBytesRcvd<sizeof(header))
	{
		/* receive header */
		numBytesRcvd = recv(sock, &(rcvHeadBuf[totalBytesRcvd]), sizeof(header)-totalBytesRcvd, 0);
		if (numBytesRcvd < 0)
	          	fatal_error("header recv failed\n");
		totalBytesRcvd += numBytesRcvd;
	}

	return (header *)rcvHeadBuf;
}

/* receive file and return the file */
FILE * receiveFile(FILE *file, int numBytesToWrite, int sock) 
{
	if(!file)
		fatal_error("file can not be received because the file is null\n");

	char *rcvBuf;			/* rcvBuf */
	
	/* Determine max buffer size */
	int bufferSize = (RCVBUFSIZE<numBytesToWrite)?RCVBUFSIZE:numBytesToWrite;
	
	/* malloc space for receive buffer */
	rcvBuf =(char *)malloc(bufferSize);
	if(!rcvBuf)
		fatal_error("malloced memory for rcvBuf failed\n");
		
	memset(rcvBuf, 0, bufferSize);
	
	int totalbytesWritten = 0;
	int currNumBytesToRcv = 0;
	int totalBytesRcvd = 0;
	int currNumBytesRcvd = 0;
	int bytesWritten = 0;
	
	/* while there are bytes still to be written */
	while(numBytesToWrite>totalbytesWritten)
	{
		/* figure out max that can be receive right now */
		currNumBytesToRcv=(numBytesToWrite-totalBytesRcvd>bufferSize)? bufferSize:numBytesToWrite-totalBytesRcvd;

		/* receive */
		currNumBytesRcvd=recv(sock, rcvBuf,currNumBytesToRcv, 0);
		if (currNumBytesRcvd < 0)
         		fatal_error("recv failed");

		/* update total bytes received */
		totalBytesRcvd+=currNumBytesRcvd;
		
		/* write received bytes to file */
		bytesWritten =fwrite(rcvBuf,1,currNumBytesRcvd,file);
		if(bytesWritten!=currNumBytesRcvd)
			fatal_error("read failed\n");	
			
		/* update total bytes written */
		totalbytesWritten+=bytesWritten;
	}		
}

/* sends file, creates buffers as needed, no limit on size it can send, closes file when done, returns number of bytes sent */
int sendFile(FILE *file, int sock)
{
	if(!file)
		return 0;
	

	/* find size of file */
	int numBytesToSend = fileLen(file);		/* total bytes that should be sent */
	if(!numBytesToSend)
		return 0;
	int bufferSize = (SNDBUFSIZE<numBytesToSend)?SNDBUFSIZE:numBytesToSend;


	/* create send buffer */
	char *sndBuf;                  
	sndBuf = (char *)malloc(bufferSize);
	if(!sndBuf)
		fatal_error("malloc memory for sndBuf failed\n");
		

	memset(sndBuf, 0, bufferSize);
	
	
	int totalFileBytesSent = 0;  			/* total bytes of the entire file that have been sent */ 
	int bytesRead = 0;  					/* total bytes that have been read thus far */
	int bytesSent = 0;  					/* total bytes that have been sent during the inner while loop, this value is reset at the beginning of the outer while loop */
	int currBytesRead=0;					/* current number of bytes read during one iteration of the outer while loop */
	int currBytesSent = 0;  				/* current number of bytes sent during one iteration of the iner while loop */
	int currNumBytesToRead = 0;				/* max number of bytes to be read in an iteration of the outer while loop */
	
	/* while the file has not been sent */
	while(numBytesToSend>totalFileBytesSent) 
	{ 

		/* checks if bytes to be read in this iteration are greater than send buffer */
		currNumBytesToRead=(numBytesToSend-bytesRead>bufferSize)?bufferSize:numBytesToSend-bytesRead;
		
 
		/* read bytes */
		currBytesRead=fread(sndBuf,1,currNumBytesToRead,file);
		if(currBytesRead<= 0)
			fatal_error("read failed\n"); 
		

		/* calculate the total number of bytes read thus far */
		bytesRead += currBytesRead;
					
		/* set the total number of bytes sent in one iteration of the outer while loop and the total number of bytes sent in one iteration of the inner while loop to 0 */
		currBytesSent = 0;
		bytesSent = 0;
		

		/* while the bytes that have been read are greater than the bytes that have been sent */
		while(bytesSent < currBytesRead)
		{
			/* send maximum number of bytes */
			currBytesSent= send(sock, &(sndBuf[bytesSent]), currBytesRead-bytesSent, 0);
			if(currBytesSent<0)
				fatal_error("send failed");
				
			bytesSent+=currBytesSent;
		}
		/* keep track of the total number of bytes that have been sent */
		totalFileBytesSent += bytesSent;
		
	}

	free(sndBuf);

	fclose(file);

	return totalFileBytesSent;
}

int logFile(char *fileName, char *method, char *ip, pthread_mutex_t *mutex, pthread_cond_t *cond, int *busy)
{
	pthread_mutex_lock(mutex);
	while((*busy)){
		pthread_cond_wait(cond,mutex);
	}
	(*busy)=1;
	pthread_mutex_unlock(mutex);	

	time_t rawtime;
	struct tm *timeptr;

	time (&rawtime);
	timeptr = localtime (&rawtime);
	char *timeStr = asctime(timeptr);
	if(!timeStr)
		fatal_error("asctime failed\n");
	

	
	char m[]="method: ";
	char ui[]=", user id: ";
	char t[]=", time: ";
	

	FILE *file=fopen(fileName,"a");
	if(!file)
		fatal_error("log file failed to open\n");
		


	if(!fwrite(m,1,strlen(m),file))
		fatal_error("failed to write to a log file\n");
	
	if(!fwrite(method,1,strlen(method),file))
		fatal_error("failed to write to a log file\n");
	
	if(!fwrite(ui,1,strlen(ui),file))
		fatal_error("failed to write to a log file\n");
	
	if(!fwrite(ip,1,strlen(ip),file))
		fatal_error("failed to write to a log file\n");
		
	if(!fwrite(t,1,strlen(t),file))
		fatal_error("failed to write to a log file\n");

	if(!fwrite(timeStr,1,strlen(timeStr),file))
		fatal_error("failed to write to a log file\n");
		
	fclose(file);	
	
	pthread_mutex_lock(mutex);
	(*busy)=0;
	pthread_mutex_unlock(mutex);
	pthread_cond_broadcast(cond);

	return 1;
	
}




