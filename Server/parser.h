#ifndef _PARSER_H_
#define _PARSER_H_

typedef enum {
    	LIST,
	DIFF,
	PULL,
	LEAVE
} method;

typedef struct {
	size_t size;
	unsigned int play_count;
	char *file_name;
} track;


#endif/*_PARSER_H_*/
