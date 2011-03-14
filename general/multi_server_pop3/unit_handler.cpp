#include "unit_data.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#define MAXDATASIZE 2048
#define MAXCOMMANDSIZE 4
#define MAXPARAMSIZE 64

char result[MAXDATASIZE];

char *command_handler(char buf[], int &q, int &user, States &state)
{	
	result[0] = '\0';
	q = 1;
	

	char command[MAXCOMMANDSIZE], param[MAXPARAMSIZE]; 
	command[0] = '\0';
	param[0] = '\0';
	sscanf(buf,"%s %s",command,param);

	
	switch (state) {
		case AUTHORIZATION:
		{
			if (strcmp(command, "USER") == 0) {
				
				bool flag = false;
				for (int i = 0; i < MAXUSERS; i++) {
					if (Massive[i].login(param) == 1) {
						if (user != -1) {
							Massive[user].unused();
							//printf("!!!!!!!/n");
						}
						
						user = i; 
						break;
					}
					if (Massive[i].login(param) == -1) {
						flag = true;
						break;
					}
					if (i == (MAXUSERS-1)) {
						flag = true;
						break;
					}
				}	
				if ((user != -1)&&(!flag)) {
					char snd[] = "+OK name is valid mailbox\n";

					strcpy(result, snd);

					state = AUTHORIZATION;
					/*
					char *usr = new char[MAXDATASIZE];
					strcpy(usr,Massive[User].user().c_str());
					printf("%s logined\n", usr);
					delete [] usr;
					*/
				}
				else
				{
					char snd[] = "-ERR wrong username\n";
					strcpy(result, snd);
				}
			} 
			else

			if (strcmp(command, "PASS") == 0) {
				
				if (user != -1) {
					if (Massive[user].pass(param) == 1) {
						char snd[] = "+OK maildrop locked and ready\n";
						strcpy(result, snd);

						state = TRANSACTION;
						/*
						char *usr = new char[MAXDATASIZE];
						strcpy(usr,Massive[User].user().c_str());
						printf("%s authorizated\n",usr);
						delete [] usr;
						*/
					}
					else
					{	
						char snd[] = "-ERR invalid password\n";
						
						strcpy(result, snd);
					}
				}		
				else
				{
					char snd[] = "-ERR client is not identified\n";
					
					strcpy(result, snd);
				}
			}
			else

			if (strcmp(command, "QUIT") == 0) {
				if (strcmp(param,"") == 0) {
					/*
					printf("client disconnect\n");
					Connected = false;
					*/
					char snd[] = "+OK dewey POP3 server\n";
					
					strcpy(result, snd);
					q = 0;
					state = UPDATE;
				}
				else
				{
					char snd[] = "-ERR invalid command\n";
				
					strcpy(result, snd);
				}
			}
			else
			{	
				char snd[] = "-ERR invalid command\n";
			
				strcpy(result, snd);
			}
			
			//printf("break switch\n");
			break;
		}
		
		case TRANSACTION:
		{	
			if (strcmp(command, "STAT") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				if (strcmp(param,"") == 0) {
					char snd[MAXDATASIZE];
					sprintf(snd, "+OK %d %d\n", Massive[user].cntmess(), Massive[user].summmess());
					strcpy(result, snd);
				}
				else
				{
					char snd[] = "-ERR invalid command\n";
					strcpy(result, snd);
				}
			} 
			else
	
			if (strcmp(command, "LIST") == 0) {
				
				if (strcmp(param,"") == 0) {

					char snd[MAXDATASIZE];
					char snd1[MAXDATASIZE];
					char snd2[MAXDATASIZE];
					char w[MAXDATASIZE];
					w[0] = '\0';
					snd[0] = '\0';
					snd1[0] = '\0';
					snd2[0] = '\0';
					sprintf(snd, "+OK %d messages (%d bytes)\n", Massive[user].cntmess(), Massive[user].summmess());
					
					//printf("!|%s|!\n",snd);
					
					for (int i=0; i<Massive[user].cntmess(); i++) {
						sprintf(snd1, "%d (%d bytes)\n", i+1, Massive[user].showmessage(i+1).length());
						//printf("|%s|\n", snd1);

						sprintf(w,"%s%s", snd2, snd1);
						strcpy(snd2, w);
					}
					
					sprintf(result,"%s%s.\n", snd, w);
				}
				else
				{
					int k = atoi(param);

					string st("");
					st = Massive[user].showmessage(k);
					
					if ( (st == "") || ( (k < 1)||(k > Massive[user].cntmess()) ) ) {
						//printf("st = empty\n");
						char snd[] = "-ERR no such message\n";
						strcpy(result, snd);
					} 
					else
					{					
						char snd[MAXDATASIZE];
						sprintf(snd,"+OK %d %d\n",k,Massive[user].showmessage(k).length());
						strcpy(result, snd);
					}
				}    
			} 
			else

			if (strcmp(command, "RETR") == 0) {
				
				int k = atoi(param);

				string st("");
				st = Massive[user].showmessage(k);
				
				if ( (st == "") || ( (k < 1)||(k > Massive[user].cntmess()) ) ) {
					char snd[] = "-ERR no such message\n";
					strcpy(result, snd);
				} 
				else
				{		
					char snd[MAXDATASIZE];
					sprintf(snd,"+OK %d bytes\n",Massive[user].showmessage(k).length());
						
					char mes[MAXDATASIZE];
					strcpy(mes,Massive[user].showmessage(k).c_str());
					
					sprintf(result, "%s%s\n.\n", snd, mes);
				}
			} 
			else

			if (strcmp(command, "DELE") == 0) {
				
				int k = atoi(param);

				string st("");
				st = Massive[user].showmessage(k);
				
				//printf("|%d|%d|\n", k, Massive[User].cntmess());

				if ( (st == "") || ( (k < 1)||(k > Massive[user].cntmess()) ) ) {
					//printf("st = empty\n");
					char snd[] = "-ERR no such message\n";
					strcpy(result, snd);
				} 
				else
				{	
					int p = Massive[user].setlabeldel(k);
					if (p == -1) {	
						char snd[MAXDATASIZE];
						sprintf(snd,"-ERR message %d already deleted\n",k);
					
						strcpy(result, snd);
					}
					if (p == 1) {
						char snd[MAXDATASIZE];
						sprintf(snd,"+OK message %d deleted\n",k);
						
						strcpy(result, snd);
					}
				}
			}
			else

			if (strcmp(command, "RSET") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				if (strcmp(param,"") == 0) {
					
					Massive[user].deletealllabels();
					char snd[MAXDATASIZE];

					sprintf(snd, "+OK maildrop has %d messages (%d bytes)\n", Massive[user].cntmess(), Massive[user].summmess());
					
					strcpy(result, snd);
				}
				else
				{
					char snd[] = "-ERR invalid command\n";
					strcpy(result, snd);
				}
			} 
			else

			if (strcmp(command, "QUIT") == 0) {
				if (strcmp(param,"") == 0) {
					
					char snd[] = "+OK dewey POP3 server\n";
					
					q = 0;
					strcpy(result, snd);
					
					state = UPDATE;
				}
				else
				{
					char snd[] = "-ERR invalid command\n";
					
					strcpy(result, snd);
				}
			}
			else

			{	
				char snd[] = "-ERR invalid command\n";
				
				strcpy(result, snd);
			}
			
			break;		
		}
		case UPDATE:
		{
			break;
		}
	}	
	
	if (state == UPDATE) {
		if (user != -1) {
			Massive[user].updatemailbox();
			Massive[user].unused();
		}
	}
	
	
	return result;
}
/*
int main()
{
	char *str;
	UsersInit();
	
	char bf1[] = "USER user0";
	int us = -1;
	int qt = 0;
	States s = AUTHORIZATION;

	str = command_handler(bf1, qt, us, s);
	printf("%s", str);


	char bf2[] = "PASS pass0";
	str = command_handler(bf2, qt, us, s);
	printf("%s", str);

	char bf3[] = "LIST";
	str = command_handler(bf3, qt, us, s);
	printf("%s", str);

	return 0;
}
*/
