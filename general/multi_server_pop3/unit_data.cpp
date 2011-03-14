#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <time.h>

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
		Person()
		{
			summ = 0;
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
		
		bool addmessage(string mes)
		{
			ExtStr elem;
			elem.st = mes;
			elem.del = false;
			messages.push_back(elem);
			summ += mes.length();
			return true;
		};

		bool updatemailbox()
		{
			vector <ExtStr>::iterator cur, w;
			for (cur = messages.begin(); cur != messages.end(); cur++) {
				if (cur->del == true) {
					summ -= cur->st.length();
					w = cur;
					cur--;
					messages.erase(w);
				}
			}
			return true;
		};
		
		string showmessage(int k)
		{
			k--;
			if (k<0 || k>=messages.size())
			    return "";
			return messages[k].st;		
		};
		
		int setlabeldel(int k)
		{
			k--;
			if (k<0 || k>=messages.size())
				return 0;
			if (messages[k].del == true) 
				return -1;
			messages[k].del = true;
			return 1;
		};

		bool deletealllabels()
		{
			vector <ExtStr>::iterator cur;
			for (cur = messages.begin(); cur != messages.end(); cur++) {
				cur->del = false;
			}
			return true;
		};

		int cntmess()
		{
			return messages.size();
		};	
		
		int summmess()
		{
			return summ;
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
	srand(time(NULL));
	for (int i=0; i < MAXUSERS; i++) {
		string st, st2, st3;
		
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
		
		int t = rand()%10;
		for (int k=0; k < t; k++) {
			char sss[5];
			sss[0] = '\0';
			sprintf(sss, "%d", k);
			st3 = sss;
			Massive[i].addmessage("user"+st2+"message"+st3);
		}
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
