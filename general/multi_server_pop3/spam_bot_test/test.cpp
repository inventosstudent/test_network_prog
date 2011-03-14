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
#define kol 1001
#define port 3490

using namespace std;


int main(int avg,char *argv[])
{	
	int c=0;
	sscanf(argv[2],"%d",&c);

	printf("\n%d\n",c);

	return 0;
}
