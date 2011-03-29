#include <iostream>
#include <event.h>
#include <evdns.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAX_LINE 66000
#define PORT 3490
#define MAXUSER 100
int fl_uk;

void do_read(int fd, short events, void *arg);
void do_write(int fd, short events, void *arg);

struct fd_state {
    char buffer[MAX_LINE];
	int acpt;
	int kl;

    struct event *read_event;
	struct event *write_event;
	char *ukr;
	char *ukw;
	FILE *fl;
};


struct fd_state *alloc_fd_state(struct event_base *base, int fd)
{
    struct fd_state *state = new struct fd_state;
    if (!state) return NULL;
	
	state->acpt=fd;
	state->fl=NULL;

	struct event *re=new (struct event);
	struct event *we=new (struct event);
	state->read_event=re;
	state->write_event=we;

    event_set(state->read_event, fd, EV_TIMEOUT|EV_READ|EV_PERSIST, do_read,(void *) state);

	if (!state->read_event) 
	{
		
        free(state);
        return NULL;
    }
	event_set(state->write_event, fd, EV_WRITE|EV_PERSIST, do_write, state);

    if (!state->write_event) 
	{
		event_del(state->read_event);        
		free(state);
        return NULL;
    }

	state->ukr=&state->buffer[0];

    return state;
}

void free_fd_state(struct fd_state *state)
{
	printf("Free\n");
	event_del(state->write_event);
	event_del(state->read_event);
	if (state->fl)fclose(state->fl);
	close(state->acpt);
	delete state->read_event;
	delete state->write_event;
	delete state;
}

void do_write(int fd, short events, void *arg)
{
	printf("sock -write %d\n",fd);	
	struct fd_state *state = (struct fd_state *)arg;

	int koef=6500;
	int k;

//	sleep(1);

	if (!state->kl)
	{
		k=fread(state->buffer,sizeof(char),koef,state->fl);
		printf("file read - %d\n",k);
		state->buffer[k]='\0';
		state->ukw=&state->buffer[0];
		state->kl=k;
	}

	printf("fd = %d\n",fd);
	int result = send(fd,state->ukw,state->kl,MSG_NOSIGNAL);
	if (result <= 0) 
	{
		if (errno == EAGAIN) return;
		printf("result < 0!!!!\n");
		free_fd_state(state);
		return;
	}

	printf("result - %d\n",result);

	state->ukw+=result;
	state->kl-=result;
	
	if (feof(state->fl)&& !state->kl)
	{
		free_fd_state(state);
		printf("ex\n");
		return;
	}	

}

int parse(struct fd_state *state)
{
	char get[1024],other[10000];
	get[0]='\0';
	other[0]='\0';
	sscanf(state->buffer,"GET %s HTTP%s\r\n%s",get,other,other);

	printf("get = %s\n",other);

	if (!strcmp(get,"/"))
	{
		char ms[]="HTTP/1.1 302 Found\r\nLocation: index.html\r\n\r\n";
		send(state->acpt,ms,strlen(ms),0);
		return 1;
	}

/*	char *q=&get[1];
	sprintf(get,"%s",q);
*/
	if (!strcmp(other,""))
	{
		printf("parse error\n");
		return 1;
	}

	if (!strcmp(get,"HTTP/1.1"))
	{
		sprintf(get,"%s","index.html");
	}
	printf("parse - |%s|%s|\n",get,other);
	FILE *fl;
	if ((fl = fopen(get,"rb"))==NULL)
	{
		fl = fopen("404.html","rb");
	}
	
	state->fl=fl;
	return 0;
}


void do_read(int fd, short events, void *arg)
{
	struct fd_state *state = (struct fd_state *)arg;
    ssize_t result;
	
	printf("!!!!!read sock -%d\n",fd);
	result = recv(state->acpt,state->ukr , 1024, 0);
	printf("%s\n",state->ukr);
	state->ukr+=result;
	*(state->ukr)='\0';
///////////
	if (result == 0 || strlen(state->buffer)>MAX_LINE-2000) 
	{
        free_fd_state(state);
		return;
    } else 
	if (result < 0) 
	{
        if (errno == EAGAIN) return;
        perror("recv");
        free_fd_state(state);
		return;
    }
//////////

	if  (strlen(state->buffer)>4)
	{
		if ((*(state->ukr-1)=='\n')&&
			(*(state->ukr-2)=='\r')&&
			(*(state->ukr-3)=='\n')&&
			(*(state->ukr-4)=='\r')
			)
			{
				printf("end zapros\n");
				int err=parse(state);
				if (err==1)
				{
					free_fd_state(state);
					char mss[]="File not founded\n";
					send(fd,mss,strlen(mss),0);
					return;
				}	
				state->kl=0;
				char mss[]="HTTP/1.1 200 OK\r\nKeep-Alive: 1115\r\nConnection: keep-alive\r\n\r\n";
//				char mss[]="HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";
				send(fd,mss,strlen(mss),0);
				event_del(state->read_event);
				event_add(state->write_event,NULL);
			}
	}
    
}


void do_accept(int listener, short event, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
/*
	int opr=send(fd,"!!!\n",4,0);
	printf("%d\n",opr);
	return;
*/
	printf("sock - %d\n",fd);
  
	if (fd < 0) 
	{
        perror("accept");
    } 
	else 
	if (fd > FD_SETSIZE) 
	{
        close(fd);  
    } 
	else 
	{
		printf("+1\n");

		struct fd_state *state;
		evutil_make_socket_nonblocking(fd);
		state = alloc_fd_state(base, fd);
		
		struct timeval tm;
		tm.tv_sec=20;
		event_add(state->read_event,NULL);
	}
}
int main(int c, char *argv[])
{
	chroot("/home/evgen/work_dir/reposit/test_network_prog/evgen/http");
	int listener;
    struct sockaddr_in sin;
    struct event_base *base;
	struct event listener_event;
	
	base = event_init();
    if (!base)
        return 0; 

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

	int one = 1;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return 0;
    }

    if (listen(listener, 10)<0) {
        perror("listen");
        return 0;
    }

    printf("!\n");
	event_set(&listener_event, listener, EV_READ|EV_PERSIST, do_accept, (void *)base);
    event_add(&listener_event, NULL);
    event_base_dispatch(base);
    
	return 0;
}
