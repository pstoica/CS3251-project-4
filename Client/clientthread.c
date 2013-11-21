#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>            /* for OpenSSL EVP digest libraries/SHA256 */
#include <time.h>			 /* for time delay */
#include <sys/types.h>		 /* files */
#include <sys/stat.h>		 /* files */ 
#include <dirent.h>					/* for determining files in current directory */
#include "networking.h"
#include "parser.h"

/* The main function */
int main(int argc, char *argv[])
{

	struct sockaddr_in serv_addr;   /* The server address */

	/* Create a new TCP socket*/
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
		fatal_error("create socket failed");


	/* Construct the server address structure */
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(2001);


	/* Establish connection to the server */
	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		fatal_error("connect failed");
	
	while(1)
	{
		int method=parser();

		if(method==LIST)
		{
			if(!clientList(sock))
				fatal_error("client list failed\n");
		}
		else if(method==DIFF)
		{
			if(!clientDiff(sock))
				fatal_error("client diff failed\n"); 
		}
		else if(method==PULL)
		{
			if(!clientPull(sock))
				fatal_error("client pull failed\n"); 
		}
		else if(method==LEAVE)
		{
			if(!clientLeave(sock))
				fatal_error("client leave failed\n");
			break;
			
		}
		else
		{
			fatal_error("method does not exist\n");
		}
	}


	/* Ending session */
	close(sock);  


	/* Freeing malloced memory */


	return 0;
}


int fatal_error(char *errormsg[])
{
	printf("%s", errormsg);
	printf("\n");
	exit(0);
}

