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

#define PORT 3490

using namespace std;

int kol=6;
bool serv_shutdown=0;


struct Tuser
{
	string nik;
	string pass;
	string mss;
};

Tuser usr[20];

//pthread_t threads[10];

void *PrintHello(void *thread)
{
//	thread_exit(NULL);
}

int main()
{
	int sock,listener;
	struct sockaddr_in addr;

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

	listen(listener,10);

	

	while(1)
	{
		if (serv_shutdown)
		{
			close(listener);
			break;
		}
		printf("do listen\n");
		sock=accept(listener,NULL,NULL);
		printf("posle listen\n");

		if (sock<0)
		{
			perror("accept");
			exit(3);
		}
		

		char buf[1024];
		
		send(sock,"+OK server ready\n",17,0);
		
		printf("+1\n");

		char name[20],word[20],word2[20],pass[20];
		int numb;
		
		memset(word,0,sizeof(word));
		memset(name,0,sizeof(name));
		memset(word2,0,sizeof(word2));
		memset(pass,0,sizeof(pass));
		
		int stage=0;
		char _buf[1024];
		
		while (1)
		{	
				memset(_buf,0,sizeof(_buf));
				memset(buf,0,sizeof(buf));
								
				memset(word,0,sizeof(word));
				memset(name,0,sizeof(name));
				memset(word2,0,sizeof(word2));
				memset(pass,0,sizeof(pass));
				
				int k=recv(sock,buf,1024,0);
				for (int i=0;i<k-2;i++)
				{
					_buf[i]=buf[i];
				}
				printf("|%s|stage=%d\n",_buf,stage);

				if (!strcmp(_buf,"QS"))
				{
					char err[]="+OK close\n";
					send(sock,err,strlen(err),0);
					close(sock);
					serv_shutdown=1;
					break;	
				}
				
				if (!strcmp(_buf,"QUIT"))
				{
					char err[]="+OK close\n";
					send(sock,err,strlen(err),0);

					close(sock);
					break;
				}
		
				sscanf(_buf,"%s %s",word,name);
				printf("|%s| |%s|\n",word, name);
				switch (stage)
				{
					case 0:
					{
						printf("\nCASE0\n");
						if (!strcmp(word,"USER"))
						{
							numb=-1;
//							send(sock,word,strlen(word),0);

							for (int i=0;i<kol;i++)
							{
								if (usr[i].nik==name)
								{
									numb=i;
									break;
								}
							}
												
							if (numb<0)
							{
								char err[]="-ERR never heard of mailbox name\n";
								send(sock,err,strlen(err),0);
							}else
							{
								char mes[]="+OK name is a valid mailbox\n";
								send(sock,mes,strlen(mes),0);
								stage++;
							}
						break;
						}
					}
					case 1:
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
						break;
						}	
					}
					case 2:
					{
						printf("\nCASE2\n");
						if (!strcmp(word,"LIST"))
						{
							char mes[]="+OK scan listing follows\n";
							send(sock,mes,strlen(mes),0);

							char mes2[]="message\n";
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
			//	}
		}
	}

	return 0;
}

