#include <sys/types.h>
#include <netinet/in.h>
//#include <errno.h>
#include "unit_multiplex.h"
//#include "unit_threads.cpp"

#define PORT 3490
#define pb push_back

int main()
{
	int listener;
	struct sockaddr_in addr;

	listener=socket(AF_INET,SOCK_STREAM,0);
	
	if (listener<0)
	{
		perror("socket");
		exit(1);
	}

	int y=1;
	setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&y,sizeof(int));	

	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	printf("Listen port: %d\n",PORT);

	if (bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("bind");
		exit(2);
	}

	UsersInit();

	int _error = work(listener);

	if (_error)
	{
		//обработка ошибок
	}
	
	return 0;
}
