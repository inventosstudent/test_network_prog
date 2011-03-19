#include "unit_multiplex.h"

int process_data(param &it)
{
	int l=strlen(it.buf);
	char *w=&it.buf[l];

	int _quit = 1;
	
	int r=recv(it.sock,w,2,0);
	if (r<=0||l+r>MAXDATASIZE-2)
	{
		strcpy(it.buf, "QUIT\r\n");
		command_handler(it.buf, _quit, it.user, it.state);
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

	
	//printf("|%s|%d|%d|\n", it.buf, _quit, it.user);
	
	char* ans=command_handler(it.buf, _quit, it.user, it.state);
	it.buf[0] = '\0';
  	
	//printf("|%s|%d|%d|\n", it.buf, _quit, it.user);
	
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

	listen(listener,BACKLOG);

	vector<param> s_sock;

	while(1)
	{
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

		if (FD_ISSET(listener,&readset))
		{
			int sock=accept(listener,NULL,NULL);
			if (sock<0)
			{
				printf("erro with accept socket\n");
				continue;
			}

			fcntl(sock,F_SETFL,O_NONBLOCK);

			char msg[]="+OK server ready\n";
			send(sock,msg,strlen(msg),0);
    
			//printf("+1\n");
			
			param ex;
			ex.sock=sock;
			ex.user=-1;
			ex.state=AUTHORIZATION;
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
