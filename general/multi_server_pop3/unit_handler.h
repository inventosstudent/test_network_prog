#ifndef _UNIT_HANDLER
#define _UNIT_HANDLER

#include <string.h>
#include "unit_data.h"

#define MAXDATASIZE 2048
#define MAXCOMMANDSIZE 4
#define MAXPARAMSIZE 64

enum States 
{
	AUTHORIZATION,
	TRANSACTION,
	UPDATE
};

char *command_handler(char buf[], int &q, int &user, States &state);

#endif
