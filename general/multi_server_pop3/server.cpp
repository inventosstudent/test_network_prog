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
#include "unit_multiplex.cpp"

#define PORT 3490
#define pb push_back

using namespace std;

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

	srand(time(NULL));//
	int k=rand()%4;//
	
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT+k);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	printf("PORT:%d\n",PORT+k);//
	
	if (bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("bind");
		exit(2);
	}

	UsersInit();

	int _error=work(listener);

	if (_error)
	{
		//обработка ошибок
	}
	
	return 0;
}
