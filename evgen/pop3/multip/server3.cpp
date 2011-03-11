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
#include <vector>
#include <errno.h>

#define PORT 3490
#define pb push_back


using namespace std;

int kol=6;
bool u[10];

struct param {
	int sock;
	int numb;
	int stage;
	char buf[4000];
};

struct Tuser
{
	string nik;
	string pass;
	string mss;
};

Tuser usr[20];

pthread_t threads[10];

int process_data(param &it)
{

	int sock=it.sock;
	int stage=it.stage;
	int numb=it.numb;
	int l=strlen(it.buf);
	char *w=&it.buf[l];

	char name[20],word[20];
    
  	memset(word,0,sizeof(word));
	memset(name,0,sizeof(name));
    
	int r=recv(it.sock,w,1024,0);
	if (r<=0||l+r>3000)
	{
		close(it.sock);
		return 1;
	}
	bool uk=0;
	for (int i=0;i<r;i++)
	{
		if (*(w+i)=='\r')
		{
			uk=1;
			*(w+i)='\0';
			break;
		}
		if(uk)break;
	}
	if (!uk)
	{
		*(w+r)='\0';
		return 0;
	}
    
	printf("|%s|stage=%d\n",it.buf,stage);
    	sscanf(it.buf,"%s %s",word,name);
    	printf("|%s| |%s|\n",word, name);
	it.buf[0]='\0';
	
	if (!strcmp(word,"QUIT"))
  	{
	  char err[]="+OK close\n";
	  send(sock,err,strlen(err),0);
	  u[numb]=0;
	  close(sock);
	  return 1;
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
					u[numb]=-1;
				  	char mes[]="+OK name is a valid mailbox\n";
				  	send(sock,mes,strlen(mes),0);
				  	stage++;
			  	}
		  	}else
			{
				char err[]="-ERR error command\n";
				send(sock,err,strlen(err),0);
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
		  char err[]="-ERR stage user\n";
		  send(sock,err,strlen(err),0);
		  break;
	  }
  }

it.stage=stage;
it.numb=numb;
return 0;
}

int main()
{
	int listener;
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
	
	fcntl(listener,F_SETFL,O_NONBLOCK);

	if (listener<0)
	{
		perror("socket");
		exit(1);
	}

	srand(time(NULL));
	int k=rand()%4;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT+k);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	printf("PORT:%d\n",PORT+k);
	
	if (bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener,5);

	vector<param> s_sock;

	while(1)
	{
		printf("do listen\n");
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(listener,&readset);

		int mx=listener;
		for (vector<param>::iterator it=s_sock.begin();it<s_sock.end();it++)
		{
			if (it->sock>mx)mx=it->sock;
			FD_SET(it->sock,&readset);
		}

		timeval tm;
		tm.tv_sec=5;
		tm.tv_usec=0;

		if (select(mx+1,&readset,NULL,NULL,NULL)<0)//timeval
		{
			perror("select");
			exit(3);
		}
		printf("select proiden!\n");

		if (FD_ISSET(listener,&readset))
		{
			printf("find listener\n");
			int sock=accept(listener,NULL,NULL);
			if (sock<0)
			{
				perror("SOCK");
				exit(3);
			}
			fcntl(sock,F_SETFL,O_NONBLOCK);

			send(sock,"+OK server ready\n",17,0);
    
			printf("+1\n");
			
			param ex;
			ex.sock=sock;
			ex.numb=-1;
			ex.stage=0;
			ex.buf[0]='\0';

			s_sock.pb(ex);
		}

		for (vector<param>::iterator it=s_sock.begin();it<s_sock.end();it++)
		{
			if (FD_ISSET(it->sock,&readset))
			{
				if (process_data(*it)==1)
				{
					s_sock.erase(it);
				}
			}
		}
	}

	return 0;
}
