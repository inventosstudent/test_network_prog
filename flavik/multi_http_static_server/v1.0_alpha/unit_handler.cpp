#include "unit_handler.h"

char resz[MAX_LENGTH_STR];

bool fill_in_buffer_from_file(struct fd_state *state)
{
	if (feof(state->f) == 1)
		return false;
	int k = fread(state->buffer, sizeof(char), MAX_LENGTH_WRT_BUF-1, state->f);
	
	state->buffer[k] = '\0';
	state->buffer_used = k;
	state->write_upto = k;
	
	return true;
}

char *request_handler(char bufz[], int &q, timeval* tm)
{		
	resz[0] = '\0';
	q = 0;

	//char sst[MAXDATASIZE],shd[MAXDATASIZE],sbd[MAXDATASIZE];

	//sscanf(bufz,"%s\r\n%s\r\n\r\n%s",sst,shd,sbd);
	
	//Handle START STRING

	//printf("|%s|\n",sst);
	char method[64], _uri[512];
	sscanf(bufz,"%s %s HTTP/1.1\r\n", method, _uri);
	char* uri = &_uri[1];

	printf("|%s|%s|\n", method, uri);

	if (strcmp(method, "GET") == 0) {
		q = 1;
		strcpy(resz,uri);
	}
	
	char *c;
	for (unsigned int i=0; i<strlen(bufz); i++) {
		c = &bufz[i];
		if (strncmp(c,"Keep-Alive: ",strlen("Keep-Alive: ")) == 0) {
			sscanf(c,"Keep-Alive: %ld\r\n",&(tm->tv_usec));
			if (tm->tv_usec > 5000)
				tm->tv_usec = 5000;
			//tm->tv_sec = 5;
		}
		if (strncmp(c, "Connection: close", strlen("Connection: close")) == 0) {
			tm->tv_usec = 0;
			tm->tv_sec = 0;
		}
	}

	//if (str)

	printf("|resz: %s|",resz);

	return resz;
}
