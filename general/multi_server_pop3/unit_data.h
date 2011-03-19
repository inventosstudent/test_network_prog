#ifndef _UNIT_DATA
#define _UNIT_DATA

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>

#define MAXUSERS 10000

using namespace std;

class Person 
{	
	struct ExtStr 
	{
		string st;
		bool del;
	};

	private:
		string username, password, message;
		vector <ExtStr> messages;
		int summ;
		bool used;

	public:
		Person();
		virtual ~Person();
		void setusername(string name);
		void setpass(string passw);
		int login(char name[]);
		int pass(char passw[]);
		bool addmessage(string mes);
		bool updatemailbox();
		string showmessage(int k);
		int setlabeldel(int k);
		bool deletealllabels();
		int cntmess();
		int summmess();
		void unused();
};

void UsersInit();

#endif
