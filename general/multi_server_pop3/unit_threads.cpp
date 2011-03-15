#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <string>
#include <string.h>
#include <pthread.h>
#include <set>
#include <fcntl.h>
#include <algorithm>
#include "unit_handler.cpp"

#define PORT 3491

using namespace std;

int kol=6;
bool u[10];

struct thr_param {
	int sock;
};

struct Tuser
{
	string nik;
	string pass;
	string mss;
};

Tuser usr[20];

pthread_t threads[10];

void *SocketWork(void *X)
{
	struct thr_param *q=(struct thr_param *)X;
   	int sock=q->sock;
	send(sock,"+OK server ready\n",17,0);
       	char name[20],word[20];
	int numb;
    
	memset(word,0,sizeof(word));
	memset(name,0,sizeof(name));
    
	char _buf[3000];

while (1)
{
	char *w=&_buf[0];
  	while (1)
  	{
		int k=recv(sock,w,1024,0);
		bool uk=0;
		for (int i=0;i<k;i++)
		{
			if (*(w+i)=='\r')
			{
				uk=1;
				*(w+i)='\0';
				break;
			}
		}
		if (uk)break;
		w+=k;
 	}


	printf("%s\n", _buf);
	int _quit = 1;
	int user = -1;
	States state = AUTHORIZATION;
	char* msg = command_handler(_buf, _quit, user, state);
	printf("%s\n", msg);
	_buf[0] = '\0';
	send(sock, msg, strlen(msg), 0);
	if (!_quit)
	{
		close(sock);		
	}
}	
pthread_exit(NULL);
}

int work(int listener)
{
	int sock;
	struct sockaddr_in addr;

	memset(u,0,sizeof(u));


	listen(listener,5);

	while(1)
	{
		sock=accept(listener,NULL,NULL);
		
		if (sock<0)
		{
			perror("accept");
			exit(3);
		}

		thr_param *p=new thr_param;
		p->sock=sock;
		int ph_err=pthread_create(&threads[0],NULL,SocketWork,(void *)p);
		if (ph_err)
		{
			printf("Erro with pthread\n");
		}
	}

	pthread_exit(NULL);

	return 0;
}
