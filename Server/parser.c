#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "parser.h"			/*structs and methods defined*/

#define BUFSIZE 10

track *track_list;
int track_count;
int *pop_order;

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



/**
 * This function currently only reads an XML file (assuming iTunes format)
 * and retrieves Title, Size, Play Count & Location for each track in the XML
 *
 * This function only needs to be called at the beginning or upon library update
 *
 * @param filename - name of the XML file to use to determine song 'popularity'
 */
void parseXML(char *file_name){
	int i;
	track_count = 0;
	// Setup file handle and open file for reading
	FILE *file = fopen(file_name, "r");
    size_t len = 0;
    ssize_t read;
    if(file == NULL){
    	printf("Open file for read error\n");
    	return -1;
    }
    
    // Setup REGEX variables and flags for smart iterative parsing
    regex_t regex;
    int reti;
    char *line;
    bool started = false;
    bool in_song = false;
    bool done = false;
    int open_dict_tags = 0;
    reti = regcomp(&regex, "<key>Tracks", 0);
    
    // Read the file while there are still unseen track entries
    //  or until file runs out of lines
    while(!done && (read = getline(&line, &len, file) != -1)){
    	// Handle any leading spaces
    	const char* firstNonSpace = line;
    	int spaces = 0;
		while(*firstNonSpace != '\0' && isspace(*firstNonSpace))
		{
		      ++firstNonSpace;
		      spaces++;
		}
		
		// Start parsing data once Tracks key is seen	
    	if(!started){
    		reti = regexec(&regex, line, 0, NULL, 0);
    		if(!reti){
    			started = true;
    		}
    	} else {
    		// Read through Tracks dictionary entries
    		reti = regcomp(&regex, "<dict>", 0);
    		reti = regexec(&regex, line, 0, NULL, 0);
    		if(!reti){
    			open_dict_tags++;
    			if(open_dict_tags == 2){
					in_song = true;
					// allocate for track entry in tracklist
					if(track_count == 0){
						track_list = (track *) malloc(sizeof(track));
					} else {
						track_list = (track *) calloc(track_list, (track_count + 1)*sizeof(track));
					}
    			}
    		} else {
				reti = regcomp(&regex, "</dict>", 0);
				reti = regexec(&regex, line, 0, NULL, 0);
				if(!reti){
					open_dict_tags--;
					if(open_dict_tags == 1){
						in_song = false;
						track_count++;
    				} else if(open_dict_tags == 0){
    					done = true;
    				}
				} else if(in_song){
					bool line_matched = false;
					char *start;
					char *end;
					for(i = 0; i < 5 && !line_matched; i++){
						// get Name, Size, Play Count & Location
							// Name, Size, or Location (file://<file_path>/<file_name>) to ID song/file?
						switch(i){
							case 0: start = "<key>Name</key><string>"; end = "</string>"; break;
							case 1: start = "<key>Size</key><integer>"; end = "</integer>"; break;
							case 2: start = "<key>Play Count</key><integer>"; end = "</integer>"; break;
							case 3: start = "<key>Location</key><string>"; end = "</string>"; break;
						}
						
						reti = regcomp(&regex, start, 0);
						reti = regexec(&regex, line, 0, NULL, 0);
						char val[256];
						memset(val, 0, sizeof(val));
						if(!reti){
							size_t val_len = strlen(line) - strlen(start) - strlen(end) - spaces - 1;
							strncpy(val, line + strlen(start) + spaces, val_len);
							
							if(i == 1 || i == 2){
								unsigned int nval = atoi(val);
								if(i == 1) track_list[track_count].size = (size_t) nval;
								else track_list[track_count].play_count = nval;
							} else {
								if(i == 3){
									// file://localhost/.../filename.ext
									// parse val to get file name from path
									char *temp = (char *) basename(val);
									char *title = (char *) malloc(sizeof(temp));
									memset(title, 0, sizeof(title));
									char *tok = strtok(temp, "%20");
									while(tok != NULL){
										title = strcat(title, " ");
										title = strcat(title, tok);
										tok = strtok(NULL, "%20");
									}
									
									track_list[track_count].file_name = (char *) malloc(sizeof(title));
									track_list[track_count].file_name = title;
									title = NULL;
								} else {
									track_list[track_count].name = (char *) malloc(sizeof(val));
									track_list[track_count].name = val;
								}
							}
							
							memset(val, 0, sizeof(val));
							line_matched = true;
						}
					}
				}
			}
		}
    }
}
