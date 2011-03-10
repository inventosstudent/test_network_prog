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
#include <vector>
#include "_multi.cpp"

using namespace std;

struct param
{
	int p1;
	int p2;
	int p3;
};
int test(param &s)
{
	s.p1=123;
	return 0;
}

int main()
{
	printf("%d\n",_sqr(2));

	return 0;
}

