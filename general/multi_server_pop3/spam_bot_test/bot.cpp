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
#include <arpa/inet.h>
//#include "unit_data.cpp"

#define pb push_back
#define kol 1010
//#define port 3493

using namespace std;

int sock[kol];

int main(int agv, char *argv[])
{
	int port=3490;
	sscanf(argv[1],"%d",&port);
	
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
//	addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
	inet_pton(AF_INET,argv[2],&addr.sin_addr);

	char buf[5000];
	int wqe=1;


//	while (1)
//	{

	printf("!!!!!\n");
			
			for (int i=0;i<kol;i++)
			{
					sock[i]=socket(AF_INET,SOCK_STREAM,0);
					buf[0]='\0';

					if (sock[i]<0)
					{
						printf("!sock!\n");
					}
					
					if (connect(sock[i],(struct sockaddr *)&addr, sizeof(addr))<0)
					{
						printf("!connect!\n");
					}
					int k=recv(sock[i],buf,5000,0);
					buf[k]='\0';
					printf("%c", buf[0]);
			}

			printf("\n");
			for (int i=0;i<kol;i++)
			{
				buf[0]='\0';
				sprintf(buf,"USER user%d\r\n",i);
				
				send(sock[i],buf,strlen(buf),0);
				int l = recv(sock[i],buf,5000,0);
				buf[l] = '\0';
				printf("%c", buf[0]);
			}
			printf("\n");
			for (int i=0;i<kol;i++)
			{
				buf[0]='\0';
				sprintf(buf,"PASS pass%d\r\n",i);
				
				send(sock[i],buf,strlen(buf),0);
				int l = recv(sock[i],buf,5000,0);
				buf[l] = '\0';
				printf("%c", buf[0]);
			}
			printf("\n");
			for (int i=0;i<kol;i++)
			{
				buf[0]='\0';
				sprintf(buf,"LIST\r\n");
				
				send(sock[i],buf,strlen(buf),0);
				int l = recv(sock[i],buf,5000,0);
				buf[l] = '\0';
				printf("%c", buf[0]);
			}
			printf("\n");
			for (int i=0;i<kol;i++)
			{
				buf[0]='\0';
				sprintf(buf,"QUIT\r\n");
				
				send(sock[i],buf,strlen(buf),0);
				int l = recv(sock[i],buf,5000,0);
				buf[l] = '\0';
				printf("%c", buf[0]);
			}
			printf("\n");
			for (int i=0;i<kol;i++)
			{
				close(sock[i]);
			}
//	}

	return 0;
}
