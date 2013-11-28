#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdbool.h>
#include <regex.h>
#include "parser.h"			/*structs and methods defined*/

#define BUFSIZE 10

/**
 * This function currently only reads an XML file (assuming iTunes format)
 * and retrieves Title, Size, Play Count & Location for each track in the XML
 *
 * This function only needs to be called at the beginning or upon library update
 *
 * @param filename - name of the XML file to use to determine song 'popularity'
 */
void parseXML(char *file_name, track *track_list){
	int i;
	int track_count = 0;
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
    size_t val_len;
    int start_len, end_len;
    char val[256];
    //track_list = (track *)malloc(12*sizeof(track));
    
    // Read the file while there are still unseen track entries
    //  or until file runs out of lines
    while((read = getline(&line, &len, file) != -1)){
    	printf("%i \n", track_count);
    	fflush(stdout);
    	// Handle any leading spaces
    	const char* firstNonSpace = line;
    	int spaces = 0;
		while(*firstNonSpace != '\0' && isspace(*firstNonSpace))
		{
		      ++firstNonSpace;
		      spaces++;
		}
		memset(val, 0, 256);
		
		reti = regcomp(&regex, "<key>Size</key><integer>", 0); 
		reti = regexec(&regex, line, 0, NULL, 0);
		//regfree(&regex);
		if(!reti){
			// get the size
			start_len = strlen("<key>Size</key><integer>");
			end_len = strlen("</integer>");
			val_len = strlen(line) - start_len - end_len - spaces - 1;
			strncpy(val, line + start_len + spaces, val_len);
			printf("%s -- ", val);
			unsigned int nval = atoi(val);
			
			track_list[track_count].size = (size_t) nval;
		} else {
			
			reti = regcomp(&regex, "<key>Play Count</key><integer>", 0);
			reti = regexec(&regex, line, 0, NULL, 0);
			//regfree(&regex);
			if(!reti){
				// get the play count
				start_len = strlen("<key>Play Count</key><integer>");
				end_len = strlen("</integer>");
				val_len = strlen(line) - start_len - end_len - spaces - 1;
				strncpy(val, line + start_len + spaces, val_len);
				printf("%s \n", val);
			
				unsigned int nval = atoi(val);
				track_list[track_count].play_count = nval;
			} else {
		
				reti = regcomp(&regex, "<key>Location</key><string>", 0); 
				reti = regexec(&regex, line, 0, NULL, 0);
				//regfree(&regex);
				if(!reti){
					// get the location
					start_len = strlen("<key>Location</key><string>");
					end_len = strlen("</string>");
					val_len = strlen(line) - start_len - end_len - spaces - 1;
					strncpy(val, line + start_len + spaces, val_len);
					printf("%s \n", val);
			
					char *temp = (char *) basename(val);
					char *title = (char *) malloc((strlen(temp) + 1)* sizeof(char));
					printf("memset title 0\n");
					memset(title, 0, (strlen(temp) + 1) * sizeof(char));
					printf("%s\n", title);
					printf("%s\n", temp);
					printf("strtok\n");

					int new_string_len = 0;
					int old_string_len = strlen(temp);
					for (int i = 0; i < old_string_len; i++) {
						if ((i + 2) < old_string_len && temp[i] == '%' && temp[i + 1] == '2' && temp[i + 2] == '0') {
							title[new_string_len] = ' ';
							i += 2;
						} else {
							title[new_string_len] = temp[i];
						}

						new_string_len++;
					}

					title[new_string_len] = '\0';
					/*
					char *tok = strtok(temp, "%20");
					while(tok != NULL){
						title = strcat(title, " ");
						title = strcat(title, tok);
						tok = strtok(NULL, "%20");
						printf("%s\n", title);
					}*/

					printf("filename %s\n", title);
					track_list[track_count].file_name = title;
					track_count++;
				}
			}
		}
    }
    
    fclose(file);
}

track *getOrderedTrackList(int numSongs, track *track_list){
	int track_count, i, j;

	printf("parse xml\n");
	parseXML("iTunes Library.xml", track_list);

	printf("list malloc & memcpy & memset\n");
	track unordered_list[numSongs];
	memcpy(&unordered_list, &track_list, sizeof(track_list));
	//memset(track_list, 0, sizeof(track)*track_count);
	int next = 0;

	for(i = 0; i < track_count; i++){
		int max_ind = 0;
		for(j = 0; j < track_count; j++){
			if(unordered_list[j].play_count > unordered_list[max_ind].play_count)
				max_ind = j;
		}
		track_list[next] = unordered_list[max_ind];
		//memcpy(&(track_list[next]), &(unordered_list[max_ind]), sizeof(track));
		unordered_list[max_ind].play_count = -1;
		next++;
	}
	
	return track_list;
}