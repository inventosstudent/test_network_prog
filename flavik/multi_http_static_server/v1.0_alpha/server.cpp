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

#define MAX_LINE 16384

void do_read(int fd, short events, void *arg);
void do_write(int fd, short events, void *arg);

char rot13_char(char c)
{
    /* We don't want to use isalpha here; setting the locale would change
     * which characters are considered alphabetical. */
/*    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
*/
        return c;
}

struct fd_state {
    char buffer[MAX_LINE];
    size_t buffer_used;

    size_t n_written;
    size_t write_upto;

    struct event *read_event;
    struct event *write_event;
};

fd_state* alloc_fd_state(event_base *base, int fd)
{
    struct fd_state *state = new (struct fd_state);
    if (!state)
        return NULL;
   
    struct event *rd = new (struct event);
    struct event *wrt = new (struct event);

	state->read_event = rd;
	state->write_event = wrt;

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

    state->buffer_used = state->n_written = state->write_upto = 0;

    assert(state->write_event);
    return state;
}

void free_fd_state(fd_state *state)
{
    event_del(state->read_event);
    event_del(state->write_event);
    delete state;
    return;
}

void do_read(int fd, short events, void *arg)
{
    fd_state *state = (fd_state *)arg;
    char buf[1024];
    int i;
    ssize_t result;

	//printf("%c read\n",state->buffer[0]);
	//sleep(1);
    //while (1) 
        assert(state->write_event);
        result = recv(fd, buf, /*2*/sizeof(buf), 0);
        //if (result <= 0)
            //continue;

        for (i=0; i < result; ++i)  {
            if (state->buffer_used < sizeof(state->buffer)) {
                state->buffer[state->buffer_used++] = rot13_char(buf[i]);
	    }
            if (buf[i] == '\n') {
                assert(state->write_event);
                event_add(state->write_event, NULL);
                state->write_upto = state->buffer_used;
		state->buffer[state->buffer_used] = '\0';
		
		event_del(state->read_event);

		//printf("read end |%s|%d|%d|\n",state->buffer, state->write_upto, state->buffer_used);
		
		break;
            }
        }
    //}

    if (result == 0) {
        free_fd_state(state);
	close(fd);
    } 
    else 
    	if (result < 0) {
        	if (errno == EAGAIN) // XXXX use evutil macro
        		return;
        	perror("recv");
        	free_fd_state(state);
		close(fd);
    	}
    return;
}

void do_write(int fd, short events, void *arg)
{
    fd_state *state = (fd_state *)arg;
	
    //printf("%c write\n",state->buffer[0]);
    //sleep(1);
    //while (state->n_written < state->write_upto) {
	ssize_t result = send(fd, state->buffer + state->n_written, state->write_upto - state->n_written, 0);
        if (result < 0) {
            if (errno == EAGAIN) // XXX use evutil macro
                return;
            free_fd_state(state);
            return;
        }
        assert(result != 0);

        state->n_written += result;
    //}

    //printf("%d %d %d %d\n", state->write_upto, state->buffer_used, state->n_written, state->write_upto - state->n_written);
    if (state->n_written >= state->buffer_used) {
    	state->n_written = state->write_upto = state->buffer_used = 0;
	state->buffer[state->buffer_used] = '\0';
	
	//printf("!!!!!!!!%c!!!!!!!!!\n",state->buffer[state->buffer_used]);
	event_add(state->read_event, NULL);
    	event_del(state->write_event);
    }
    return;
}

void do_accept(int listener, short event, void *arg)
{
    event_base *base = (event_base *)arg;
    sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

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
		
		state = alloc_fd_state(base, fd);
	       
		assert(state); 
		assert(state->write_event);
	       
		event_add(state->read_event, NULL);
    	}
}


int main(int c, char **v)
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
	sin.sin_port = htons(1111);

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
	event_base_dispatch(base);


	return 0;
}
