#ifndef _UNIT_MULTIPLEX
#define _UNIT_MULTIPLEX

#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include "unit_handler.h"

#define pb push_back
#define BACKLOG 1010
struct param {
	int sock;
	int user;
	States state;
	char buf[MAXDATASIZE];
};

int process_data(param &it);

int work(int listener);

#endif
