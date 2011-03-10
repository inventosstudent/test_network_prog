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
	printf("Enter\n");
    send(sock,"+OK server ready\n",17,0);
    
    printf("+1\n");

   	int stage=0; 

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

    printf("|%s|stage=%d\n",_buf,stage);
    sscanf(_buf,"%s %s",word,name);
    printf("|%s| |%s|\n",word, name);

	if (!strcmp(word,"QUIT"))
  	{
	  char err[]="+OK close\n";
	  send(sock,err,strlen(err),0);
	  close(sock);
	  if (numb>=0)u[numb]=0;
	  break;
  	}
  
  switch (stage)
  {
	  case (0):
	  {
		  printf("\nCASE0\n");
		  if (!strcmp(word,"USER"))
		  {
			  numb=-1;

			  for (int i=0;i<kol;i++)
			  {
				  if (!u[i]&&usr[i].nik==name)
				  {
					  numb=i;
					  break;
				  }
			  }
								  
			  if (numb<0)
			  {
				  char err[]="-ERR never heard of mailbox name or use now\n";
				  send(sock,err,strlen(err),0);
			  }else
			  {
				  u[numb]=1;
				  char mes[]="+OK name is a valid mailbox\n";
				  send(sock,mes,strlen(mes),0);
				  stage++;
			  }
		  }else
		  {
				  char err[]="-ERR error command\n";
				  send(sock,err,strlen(err),0);
				  break;
		  }
		  break;
		  
	  }
	  case (1):
	  {
		  printf("\nCASE1 |%d|\n",numb);
		  
		  if (!strcmp(word,"PASS"))
		  {
			  if (usr[numb].pass==name)
			  {
				  char mes[]="+OK maildrop locked and ready\n";
				  send(sock,mes,strlen(mes),0);
				  stage++;
			  }else
			  {
				  char err[]="-ERR invalid password\n";
				  send(sock,err,strlen(err),0);
			  }
		  }else
		  {
				  char err[]="-ERR error command\n";
				  send(sock,err,strlen(err),0);
				  break;
		  }
		  break;
			
	  }
	  case (2):
	  {
		  printf("\nCASE2\n");
		  if (!strcmp(word,"LIST"))
		  {
			  char mes[]="+OK scan listing follows\n";
			  send(sock,mes,strlen(mes),0);

			  char mes2[3000];
			  int l=(int)usr[numb].mss.length();
			  for (int i=0;i<l;i++)
			  {
				mes2[i]=usr[numb].mss[i];
			  }
			  mes2[l]='\n';
			  mes2[l+1]='\0';
			  send(sock,mes2,strlen(mes2),0);
		  }else
		  {
			  char mes[]="-ERR no such command\n";
			  send(sock,mes,strlen(mes),0);
		  }

		  break;
	  }
	  default:
	  {
		  char err[]="-ERR error command\n";
		  send(sock,err,strlen(err),0);
		  break;
	  }
  }
}
pthread_exit(NULL);
}

int main()
{
	int listener,sock;
	struct sockaddr_in addr;

	memset(u,0,sizeof(u));

	usr[0].nik="user0";
	usr[0].pass="pass0";
	usr[0].mss="message0";

	usr[1].nik="user1";
	usr[1].pass="pass1";
	usr[1].mss="message1";
	
	usr[2].nik="user2";
	usr[2].pass="pass2";
	usr[2].mss="message2";

	usr[3].nik="user3";
	usr[3].pass="pass3";
	usr[3].mss="message3";

	usr[4].nik="user4";
	usr[4].pass="pass4";
	usr[4].mss="message4";

	usr[5].nik="user5";
	usr[5].pass="pass5";
	usr[5].mss="message5";

	listener=socket(AF_INET,SOCK_STREAM,0);
	
	if (listener<0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	if (bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener,5);

	while(1)
	{
		printf("do listen\n");
		sock=accept(listener,NULL,NULL);
		printf("posle listen\n");

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
