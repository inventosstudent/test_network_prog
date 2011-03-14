#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

#define MAXUSERS 1000

using namespace std;

enum States 
{
	AUTHORIZATION,
	TRANSACTION,
	UPDATE
};

class Person 
{
	private:
		string username, password, message;
		bool used;
	public:
		Person()
		{
			used = false;
		};

		virtual ~Person()
		{
		};
		
		void setusername(string name)
		{
			username = name;
			return;
		};
				
		void setpass(string passw)
		{
			password = passw;
			return;
		};

		void setmessage(string mess)
		{
			message = mess;
			return;
		};

		int login(char name[])
		{
			if (name == username) {
				if (used == false) {
					used = true;
					return 1;
				}
				else
					return -1;
			}
			return 0;
		};

		int pass(char passw[])
		{
			if (passw == password) 
				return 1;
			return 0;
		};

		string showmessage()
		{
			return message;		
		};
		
		void unused()
		{
			used = false;
			return;
		};
};

Person Massive[MAXUSERS];

void UsersInit()
{
	for (int i=0; i < MAXUSERS; i++) {
		string st, st2;
		
		char ss[5];
		ss[0] = '\0';
		sprintf(ss,"%d",i);
		
		st2 = ss;
		st = "user"+st2;
		Massive[i].setusername(st);
		st = "";

		st = "pass"+st2;
		Massive[i].setpass(st);
		st = "";

		st = "message"+st2;
		Massive[i].setmessage(st);
	}
	return;
}
/*
int main()
{
	UsersInit();
	return 0;
}
*/
//}
