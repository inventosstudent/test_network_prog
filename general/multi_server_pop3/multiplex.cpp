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
#define SIZEBUFF 4000


using namespace std;

struct param {
	int sock;
	int user;
	States state;
	char buf[SIZEBUFF];
};


int process_data(param &it)
{
	int l=strlen(it.buf);
	char *w=&it.buf[l];

	int r=recv(it.sock,w,2,0);
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

	int _quit;
	char* ans=command_handler(char it.buf,int &_quit,int it.user,States &it.state);
	delete [] ans;
  	
	send(it.sock,ans,strlen(ans),0);
	
	if (!_quit)
  	{
	  close(it.sock);
	  return 1;
  	}

return 0;
}

int work(int listener)
{
	fcntl(listener,F_SETFL,O_NONBLOCK);

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
				printf("erro with accept sock");
				continue;
			}

			fcntl(sock,F_SETFL,O_NONBLOCK);

			char msg[]="+OK server ready\n";
			send(sock,msg,strlen(msg),0);
    
			printf("+1\n");
			
			param ex;
			ex.sock=sock;
			ex.user=-1;
			ex.state=States[0];
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
