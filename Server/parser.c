#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "parser.h"			/*structs and methods defined*/

#define BUFSIZE 10

track *track_list;
int track_count, i, j;

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
    	return;
    }
    
    // Setup REGEX variables and flags for smart iterative parsing
    regex_t regex;
    int reti;
    char *line;
    bool started = false;
    bool in_song = false;
    bool done = false;
    int open_dict_tags = 0;
    bool line_matched = false;
	char val[256];
	int start_len, end_len;
	char *temp;
	char *tok;
	char* firstNonSpace;
    
    // Read the file while there are still unseen track entries
    //  or until file runs out of lines
    while(!done && (read = getline(&line, &len, file) != -1)){
    	// Handle any leading spaces
    	//printf("%s\n", line);
    	firstNonSpace = line;
    	int spaces = 0;
		while(*firstNonSpace != '\0' && isspace(*firstNonSpace))
		{
		      ++firstNonSpace;
		      spaces++;
		}
		
		// Start parsing data once Tracks key is seen	
    	if(!started){
    		reti = regcomp(&regex, "<key>Tracks", 0);
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
						printf("initial list malloc\n");
						track_list = (track *) malloc(sizeof(track));
					} else {
						printf("list realloc\n");
						track_list = (track *) realloc(track_list, (track_count + 1)*sizeof(track));
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
    				regfree(&regex);
				} else if(in_song){
					line_matched = false;

					for(i = 1; i < 4 && !line_matched; i++){
						printf("loop -- %i -- \n", i);
						// get Size, Play Count & Location
						switch(i){
							//case 0: start = "<key>Name</key><string>"; end = "</string>"; break;
							case 1: reti = regcomp(&regex, "<key>Size</key><integer>", 0); 
									start_len = strlen("<key>Size</key><integer>");
									end_len = strlen("</integer>"); break;
							case 2: reti = regcomp(&regex, "<key>Play Count</key><integer>", 0); 
									start_len = strlen("<key>Play Count</key><integer>");
									end_len = strlen("</integer>"); break;
							case 3: reti = regcomp(&regex, "<key>Location</key><string>", 0); 
									start_len = strlen("<key>Location</key><string>");
									end_len = strlen("</string>"); break;
						}
						
						printf("set values...regex\n");
						reti = regexec(&regex, line, 0, NULL, 0);
						
						memset(val, 0, 256);
						if(!reti){
							size_t val_len = strlen(line) - start_len - end_len - spaces - 1;
							strncpy(val, line + start_len + spaces, val_len);
							printf("%s \n", val);
							
							if(i == 1 || i == 2){
								printf("got size or play_count");
								unsigned int nval = atoi(val);
								if(i == 1) track_list[track_count].size = (size_t) nval;
								else track_list[track_count].play_count = nval;
								printf(" -- and set it to %i\n", nval);
							} else {
								if(i == 3){
									// file://localhost/.../filename.ext
									// parse val to get file name from path
									printf("getting filename -- basename\n");
									temp = (char *) basename(val);
									printf("title malloc\n");
									char *title = (char *) malloc(sizeof(temp));
									printf("memset 0\n");
									memset(title, 0, sizeof(title));
									printf("strtok\n");
									tok = strtok(temp, "%20");
									while(tok != NULL){
										title = strcat(title, " ");
										title = strcat(title, tok);
										tok = strtok(NULL, "%20");
									}
									
									printf("filename %s\n", title);
									track_list[track_count].file_name = title;// (char *) malloc(sizeof(title));
									//memcpy(track_list[track_count].file_name, title, sizeof(title));
								}
							}
							
							printf("line_matched == true\n");
							line_matched = true;
						}
					}
				}
			}
		}
		regfree(&regex);
    }
    fclose(file);
}

track *getOrderedTrackList(){
	if(track_list == NULL){	
		printf("parse xml\n");
		parseXML("iTunes Library.xml");
	
		printf("list malloc & memcpy & memset\n");
		track *unordered_list = (track *) malloc(track_count*sizeof(track));
		memcpy(unordered_list, track_list, sizeof(track)*track_count);
		memset(track_list, 0, sizeof(track)*track_count);
		int next = 0;

		for(i = 0; i < track_count; i++){
			int max_ind = 0;
			for(j = 0; j < track_count; j++){
				if(unordered_list[j].play_count > unordered_list[max_ind].play_count)
					max_ind = j;
			}
			memcpy(&(track_list[next]), &(unordered_list[max_ind]), sizeof(track));
			unordered_list[max_ind].play_count = -1;
			next++;
		}
	
	}
	
	return track_list;
}

int getTrackCount(){
	return track_count;
}

