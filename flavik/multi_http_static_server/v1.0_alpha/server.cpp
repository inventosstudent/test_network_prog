/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <event.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "unit_handler.h"

#define PORT 1111

extern char resz[MAX_LENGTH_STR];

void do_timer(int fd, short events, void* arg);
void do_read(int fd, short events, void* arg);
void do_write(int fd, short events, void* arg);

fd_state* alloc_fd_state(event_base* base, int fd)
{
    struct fd_state* state = new (struct fd_state);
    if (!state)
        return NULL;
   
    state->read_event = new (struct event);
    state->write_event = new (struct event);
    state->time_event = new (struct event);
    state->keep_alive = new (struct timeval);
	
    state->keep_alive->tv_sec = 0;
    state->keep_alive->tv_usec = 120;
	
    event_set(state->read_event, fd, EV_READ|EV_PERSIST, do_read, state);
    if (!state->read_event) {
        delete state;
        return NULL;
    }

    event_set(state->write_event, fd, EV_WRITE|EV_PERSIST, do_write, state);
    if (!state->write_event) {
        event_del(state->read_event);
        delete state;
        return NULL;
    }

    event_set(state->time_event, fd, EV_TIMEOUT|EV_PERSIST, do_timer, state);
    if (!state->time_event) {
    	event_del(state->read_event);
	event_del(state->write_event);
	delete state;
	return NULL;
    }

    state->buffer_used = state->n_written = state->write_upto = 0;
    state->f = NULL;

    assert(state->write_event);
    return state;
}

void free_fd_state(fd_state* state)
{
    event_del(state->read_event);
    event_del(state->write_event);
    event_del(state->time_event);
    delete state->keep_alive; 
    delete state->read_event;
    delete state->write_event;
    delete state;
    return;
}

void do_timer(int fd, short events, void* arg)
{
	fd_state* state = (fd_state *)arg;
	close(fd);
	printf("do_timer EXECUTED! Keep-Alive: %ld\n",state->keep_alive->tv_usec);
	
	free_fd_state(state);

	return;
}

void do_read(int fd, short events, void* arg)
{
    fd_state* state = (fd_state *)arg;
    char buf[MAX_LENGTH_RD_BUF];
    ssize_t result;

	printf("reading\n");
	//printf("%c read\n",state->buffer[0]);
	//sleep(1);
    //while (1) 
        assert(state->write_event);
        result = recv(fd, buf, /*2*/sizeof(buf), 0);
	
	    if (result == 0) {
		free_fd_state(state);
		close(fd);
		return;
	    } 
	    else 
		if (result < 0) {
			if (errno == EAGAIN) // XXXX use evutil macro
				return;
			perror("recv");
			free_fd_state(state);
			close(fd);
			return;
		}
	buf[result] = '\0';
        //if (result <= 0)
            //continue;
/*	printf("|");
	for (int i=0; i<strlen(buf); i++) {
		printf("!%d!",buf[i]);
	}
	printf("|\n");

	char c1 = '\r';
	char c2 = '\n';
	printf("|!%d!!%d!|\n", c1, c2);
*/
        for (int i=0; i < result; ++i)  {
            /*
	    if (state->buffer_used < sizeof(state->buffer)) {
                state->buffer[state->buffer_used++] = rot13_char(buf[i]);
	    }
	    */
            if (((i+3) < result)&&(buf[i] == '\r')&&(buf[i+1] == '\n')&&(buf[i+2] == '\r')&&(buf[i+3] == '\n')) {

                assert(state->write_event);
		event_del(state->time_event);
		event_del(state->read_event);


		printf("enter in the request_handler\n");
		int q;
		request_handler(buf, q, state->keep_alive);
		
		if (q == 1) {
			printf("\nenter in the transfer\n");
	
			state->f = NULL;
			state->f = fopen(resz, "rb");
			if (!state->f) {	
				printf("state->f == NULL \n");
				strcpy(state->buffer, "HTTP/1.1 404 Not Found\r\nServer: http_server v0.5\r\n\r\n"
						      "<html><center>404. FILE NOT FOUND</center></html>");
				state->buffer_used = strlen(state->buffer);
				event_add(state->write_event, NULL);
			}
			else
			{
				strcpy(state->buffer, "HTTP/1.1 200 OK\r\nServer: http_server v0.5\r\n\r\n");
				state->buffer_used = strlen(state->buffer);
				event_add(state->write_event, NULL);
			}
		}
		
		state->n_written = 0;
                state->write_upto = state->buffer_used;
		state->buffer[state->buffer_used] = '\0';

		//printf("read end |%s|%d|%d|\n",state->buffer, state->write_upto, state->buffer_used);
		
		break;
            }
        }
    //}

    return;
}

void do_write(int fd, short events, void* arg)
{
    fd_state* state = (fd_state*)arg;
		
    //sleep(1);
    //while (state->n_written < state->write_upto) {
	ssize_t result = send(fd, state->buffer + state->n_written, state->write_upto - state->n_written, MSG_NOSIGNAL);
        if (result == 0) {
		close(fd);
		free_fd_state(state);
		return;
	}
	if (result < 0) {
            if (errno == EAGAIN) // XXX use evutil macro
                return;
	    close(fd);
            free_fd_state(state);
            return;
        }
        assert(result != 0);

        state->n_written += result;
	//printf("%s", state->buffer);
    //}

    //printf("%d %d %d %d\n", state->write_upto, state->buffer_used, state->n_written, state->write_upto - state->n_written);
    if (state->n_written >= state->buffer_used) {
    	state->n_written = state->write_upto = state->buffer_used = 0;
	state->buffer[state->buffer_used] = '\0';
	
	printf("the end of sending buffer\n");
	if (state->f) {
		bool b = fill_in_buffer_from_file(state);
		if (b == true)
			printf("b = TRUE\n");
		else
			printf("b = FALSE\n");
		if (!b) {
			fclose(state->f);
			event_del(state->write_event);
			event_add(state->read_event, NULL);
			event_add(state->time_event, state->keep_alive);
		}
	}
	else
	{
		event_del(state->write_event);
		event_add(state->read_event, NULL);
		event_add(state->time_event, state->keep_alive);
		//close(fd);
		//free_fd_state(state);
	}
    }
    return;
}

void do_accept(int listener, short event, void* arg)
{
    event_base* base = (event_base*)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    printf("accept connection with sock\n");

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) { // XXXX eagain??
        perror("accept");
    } 
    else 
    	if (fd > FD_SETSIZE) {
       		close(fd); 
	} 
	else 
	{
		fd_state *state;
		evutil_make_socket_nonblocking(fd);
	
		printf("allocate memory for state\n");
		state = alloc_fd_state(base, fd);
	       
		assert(state); 
		assert(state->write_event);
	       
		event_add(state->read_event, NULL);
		event_add(state->time_event, state->keep_alive);
    	}

	return;
}


int main(int c, char** v)
{
	setvbuf(stdout, NULL, _IONBF, 0);

	int listener;
	struct sockaddr_in sin;
	struct event_base *base;
	struct event listener_event;

	base = event_init();
	if (!base)
		return -1; /*XXXerr*/

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(PORT);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	evutil_make_socket_nonblocking(listener);

	#ifndef WIN32
	{
		int one = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
	#endif

	if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
		perror("bind");
		return -1;
	}

	if (listen(listener, 16)<0) {
		perror("listen");
		return -1;
	}


	event_set(&listener_event, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
	event_add(&listener_event, NULL);

	//eternal cycle
	event_dispatch();

	return 0;
}
