#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc,char *arg[])
{
	int sock;
	struct sockaddr_in addr;
	
	sock=socket(AF_INET,SOCK_STREAM,0);
	if (sock<0)
	{
		perror("socket");
		exit(1);
	}
	
	if (argc!=2)
	{
		perror("arg");
		exit(1);
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(3490);
	addr.sin_addr.s_addr=inet_addr(arg[1]);
	if (connect(sock,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("connect");
		exit(2);
	}
		
	char buf[12];
	recv(sock,buf,12,0);
	printf("%s\n",buf);
	close(sock);

	return 0;
}
