#include "unit_data.h"

Person Massive[MAXUSERS];

Person::Person()
{
	summ = 0;
	used = false;
}

Person::~Person()
{
}

void Person::setusername(string name)
{
	username = name;
	return;
}
		
void Person::setpass(string passw)
{
	password = passw;
	return;
}

int Person::login(char name[])
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
}

int Person::pass(char passw[])
{
	if (passw == password) 
		return 1;
	return 0;
}

bool Person::addmessage(string mes)
{
	ExtStr elem;
	elem.st = mes;
	elem.del = false;
	messages.push_back(elem);
	summ += mes.length();
	return true;
}

bool Person::updatemailbox()
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
}

string Person::showmessage(int k)
{
	k--;
	if ((k<0)||( (unsigned int)k >=messages.size()))
	    return "";
	return messages[k].st;		
}

int Person::setlabeldel(int k)
{
	k--;
	if ((k<0) || ((unsigned int)k>=messages.size()))
		return 0;
	if (messages[k].del == true) 
		return -1;
	messages[k].del = true;
	return 1;
}

bool Person::deletealllabels()
{
	vector <ExtStr>::iterator cur;
	for (cur = messages.begin(); cur != messages.end(); cur++) {
		cur->del = false;
	}
	return true;
}

int Person::cntmess()
{
	return messages.size();
}	

int Person::summmess()
{
	return summ;
}

void Person::unused()
{
	used = false;
	return;
}

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
}
