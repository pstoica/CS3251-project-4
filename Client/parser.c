#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"			/*structs and methods defined*/

#define BUFSIZE 10
#define INPUT_BUFFER_MAX 32

int parser()
{
	char *input = (char *) malloc(INPUT_BUFFER_MAX);
	char list[BUFSIZE] = "LIST";
	char diff[BUFSIZE] = "DIFF";
	char pull[BUFSIZE] = "PULL";
	char leave[BUFSIZE] = "LEAVE";
	//method currMethod;
	int givenMethod = -1;
	
	//Loop till the user gives valid input
	while (givenMethod == -1)
	{
		//Get the method to run from the user
		printf("Enter LIST, DIFF, PULL, or LEAVE (In all CAPS):");
		fgets(input, INPUT_BUFFER_MAX, stdin);

		size_t length = strlen(input) - 1;
		if (input[length] == '\n') input[length] = '\0';
		
		//Check the input of the user against all 4 methods
		if (strlen(input)<BUFSIZE&&strncmp(input, "LIST", BUFSIZE) == 0)
			givenMethod = 0;
		else if (strlen(input)<BUFSIZE&&strncmp(input, "DIFF", BUFSIZE) == 0)
			givenMethod = 1;
		else if (strlen(input)<BUFSIZE&&strncmp(input, "PULL", BUFSIZE) == 0)
			givenMethod = 2;
		else if (strlen(input)<BUFSIZE&&strncmp(input, "LEAVE", BUFSIZE) == 0)
			givenMethod = 3;
		else
			printf("Invalid input\n");
	}
	return givenMethod;
}
