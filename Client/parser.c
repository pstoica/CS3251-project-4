#include <stdio.h>
#include <string.h>
#include "parser.h"			/*structs and methods defined*/

#define BUFSIZE 10

int parser()
{
	char *input;
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
		scanf("%as", &input);
		
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
