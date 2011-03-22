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

#define MAX_LINE 16384
#define PORT 3490
int fl_uk;
void do_read(int fd, short events, void *arg);
void do_write(int fd, short events, void *arg);

struct ans{
	char body[100];
	char head[100];
};

struct fd_state {
    char buffer[MAX_LINE];
	ans repose;
	int acpt;
	FILE *fl;
	int kl;

    struct event *read_event;
	struct event *write_event;
	char *ukr;
};


struct fd_state *alloc_fd_state(struct event_base *base, int fd)
{
    struct fd_state *state = new struct fd_state;
    if (!state) return NULL;
	
	state->acpt=fd;

	struct event *re=new (struct event);
	struct event *we=new (struct event);
	state->read_event=re;
	state->write_event=we;

    event_set(state->read_event, fd, EV_READ|EV_PERSIST, do_read,(void *) state);

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
	fclose(state->fl);
	close(state->acpt);
	delete state;
}

void do_write(int fd, short events, void *arg)
{
    struct fd_state *state = (struct fd_state *)arg;
	
//	printf("peredal %s\n",uk);
	if (state->kl==0)
	{
		char mss[]="HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: 178245\r\n\r\n";
		send(fd,mss,strlen(mss),0);
	}
	
	char buf[110];
	int koef=100;
	
	int k=fread(buf,sizeof(char),koef,state->fl);
	
	FILE *qwe=fopen("test.jpg","ab");
	fwrite(buf,sizeof(char),koef,qwe);
	fclose(qwe);
	
	buf[k]='\0';
//	printf("write - %s\n",buf);	
	int result = send(state->acpt,buf,strlen(buf),0);
	
	if (feof(state->fl))
	{
		free_fd_state(state);
		printf("ex\n");
		return;
	}
	
	state->kl+=result;

//	state->ukr=uk;
//	state->ukw=&(state->repose.body[0]);
	
//	sleep(1);

//	printf("write - %c\n",*(state->ukw));
	
	if (result < 0) 
	{
		if (errno == EAGAIN) return;
		free_fd_state(state);
		return;
	}
//    state->ukw += result;

//	if (state->ukr<=state->ukw)
    	
}

int parse(struct fd_state *state)
{
	char get[1024],other[10000];
	get[0]='\0';
	other[0]='\0';
	sscanf(state->buffer,"GET /%s HTTP/1.1\r\n%s",get,other);

	if (!strcmp(get,"HTTP/1.1"))
	{
		sprintf(get,"%s","index.html");
	}
	printf("|%s|%s|\n",get,other);
	FILE *fl;
	if ((fl = fopen(get,"rb"))==NULL)
	{
		fl = fopen("404.html","rb");
	}
	
	state->fl=fl;
//	printf("|%s|",state->repose.body);

/*	other[0]='\0';
	char *uk=&other[0];
	fl=fopen("1.jpg","rb");
	FILE *fw=fopen("2.jpg","wb");
	int w;
	while (fread(other,sizeof(char),1,fl)>0)
	{
		fwrite(other,sizeof(char),1,fw);		
	}

	fclose(fw);
	fclose(fl);

	assert(0);*/
	return 0;
}


void do_read(int fd, short events, void *arg)
{
    struct fd_state *state = (struct fd_state *)arg;
    ssize_t result;

//	sleep(1);

//	printf("!\n");
	result = recv(state->acpt,state->ukr , 1024, 0);
	printf("read - |%s|\n",state->buffer);

	state->ukr+=result;
//	printf("---------------\n");

	if  (strlen(state->buffer)>4)
	{
		if ((*(state->ukr-1)=='\n')&&
			(*(state->ukr-2)=='\r')&&
			(*(state->ukr-3)=='\n')&&
			(*(state->ukr-4)=='\r')
			)
			{
				int err=parse(state);
				if (err==1)
				{
					free_fd_state(state);
					char mss[]="File not founded\n";
					send(fd,mss,strlen(mss),0);
					return;
				}	
				state->kl=0;
				event_add(state->write_event,NULL);
				event_del(state->read_event);
			}
	}
    
	if (result == 0) 
	{
        free_fd_state(state);
    } else 
	if (result < 0) 
	{
        if (errno == EAGAIN) return;
        perror("recv");
        free_fd_state(state);
    }
}


void do_accept(int listener, short event, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
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
		event_add(state->read_event, NULL);
	}
}
int main(int c, char *argv[])
{
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
