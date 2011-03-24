#ifndef _UNIT_HANDLER
#define _UNIT_HANDLER

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <event.h>
#include <time.h>

#define MAX_LENGTH_STR 1024
#define MAX_LENGTH_WRT_BUF 16384
#define MAX_LENGTH_RD_BUF 65536

struct fd_state {
    char buffer[MAX_LENGTH_WRT_BUF];
    
    FILE *f;
    
    size_t buffer_used;

    size_t n_written;
    size_t write_upto;

    struct timeval* keep_alive;
    bool connection;

    struct event* read_event;
    struct event* write_event;
    struct event* time_event; 
};

bool fill_in_buffer_from_file(fd_state *state);
char* request_handler(char bufz[], int &q, timeval* tm);

#endif
