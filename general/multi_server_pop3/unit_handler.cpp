#include "unit_data.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#define MAXDATASIZE 2048
#define MAXCOMMANDSIZE 4
#define MAXPARAMSIZE 64

//using namespace data_space;

enum States 
{
	AUTHORIZATION,
	TRANSACTION,
	UPDATE
};

char result[MAXDATASIZE];

char *command_handler(char buf[], int &q, int &user, States &state)
{	
	result[0] = '\0';
	//char *result = new char[MAXDATASIZE];
	q = 1;
	

	char command[MAXCOMMANDSIZE], param[MAXPARAMSIZE]; 
	command[0] = '\0';
	param[0] = '\0';
	sscanf(buf,"%s %s",command,param);

	//printf("|%s||%s|\n", command, param);

	
	switch (state) {
		case AUTHORIZATION:
		{
			//printf("AUTH\n");
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
				/*
				if (flag) {
					printf("%d\n", user);
				}
				*/
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
				
				//printf("!!!!\n");
				
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
		/*	
			if (strcmp(command, "STAT") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				if (strcmp(param,"") == 0) {
					char *snd = new char[MAXDATASIZE];
					sprintf(snd, "+OK %d %d\n", Massive[User].cntmess(), Massive[User].summmess());
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					} 
					delete [] snd;
				}
				else
				{
					char snd[] = "-ERR invalid command\n";
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
				}
			} 
			else
		*/	
			if (strcmp(command, "LIST") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				if (strcmp(param,"") == 0) {
					/*
					char *snd = new char[MAXDATASIZE];
					sprintf(snd, "+OK %d messages (%d bytes)\n", Massive[User].cntmess(), Massive[User].summmess());
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					} 

					for (int i=0; i<Massive[User].cntmess(); i++) {
						sprintf(snd, "%d (%d bytes)\n", i+1, Massive[User].showmessage(i+1).length());
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						} 
					}

					if (send(*psock, ".\n", 2, 0) == -1) {
						perror("send");
					} 
					delete [] snd;
					*/
					char *snd = new char[MAXDATASIZE];
					strcpy(snd, Massive[user].showmessage().c_str());
					
					sprintf(result, "%s\n.\n",snd);
					delete [] snd;
				}/*
				else
				{
					//printf("!!!!!\n");

					int k = atoi(param);

					//printf("|%d|\n",k);

					string st("");
					st = Massive[User].showmessage(k);
					
					//printf("|%d|%d|\n", k, Massive[User].cntmess());

					if ( (st == "") || ( (k < 1)||(k > Massive[User].cntmess()) ) ) {
						//printf("st = empty\n");
						char snd[] = "-ERR no such message\n";
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}
					} 
					else
					{					
						char *snd = new char[MAXDATASIZE];
						sprintf(snd,"+OK %d %d\n",k,Massive[User].showmessage(k).length());

						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}
						delete [] snd;
					}
				}    */
			} /*
			else

			if (strcmp(command, "RETR") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				//printf("!!!!!\n");

				int k = atoi(param);

				//printf("|%d|\n",k);

				string st("");
				st = Massive[User].showmessage(k);
				
				//printf("|%d|%d|\n", k, Massive[User].cntmess());

				if ( (st == "") || ( (k < 1)||(k > Massive[User].cntmess()) ) ) {
					//printf("st = empty\n");
					char snd[] = "-ERR no such message\n";
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
				} 
				else
				{		
					char *snd = new char[MAXDATASIZE];
					sprintf(snd,"+OK %d bytes\n",Massive[User].showmessage(k).length());
						
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
					delete [] snd;

					char *mes = new char[MAXDATASIZE];
					strcpy(mes,Massive[User].showmessage(k).c_str());

					if (send(*psock, mes, strlen(mes), 0) == -1) {
						perror("send");
					}
					delete [] mes;

					if (send(*psock, "\n.\n", 3, 0) == -1) {
						perror("send");
					}
				}
			}
			else

			if (strcmp(command, "DELE") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				//printf("!!!!!\n");

				int k = atoi(param);

				//printf("|%d|\n",k);

				string st("");
				st = Massive[User].showmessage(k);
				
				//printf("|%d|%d|\n", k, Massive[User].cntmess());

				if ( (st == "") || ( (k < 1)||(k > Massive[User].cntmess()) ) ) {
					//printf("st = empty\n");
					char snd[] = "-ERR no such message\n";
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
				} 
				else
				{	
					int p = Massive[User].setlabeldel(k);
					if (p == -1) {	
						char *snd = new char[MAXDATASIZE];
						sprintf(snd,"-ERR message %d already deleted\n",k);
						
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}
						delete [] snd;
					}
					if (p == 1) {
						char *snd = new char[MAXDATASIZE];
						sprintf(snd,"+OK message %d deleted\n",k);
						
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}
						delete [] snd;
					}
				}
			}
			else

			if (strcmp(command, "RSET") == 0) {
				
				//printf("|%s|%s|\n",command,param);
					
				if (strcmp(param,"") == 0) {
					Massive[User].deletealllabels();
					char *snd = new char[MAXDATASIZE];
					sprintf(snd, "+OK maildrop has %d messages (%d bytes)\n", Massive[User].cntmess(), Massive[User].summmess());
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					} 
					delete [] snd;
				}
				else
				{
					char snd[] = "-ERR invalid command\n";
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
				}
			} */
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
			//Massive[User].updatemailbox();
			Massive[user].unused();
		}
	}
	//printf("before connected\n");	
	//if (!Connected) {break;}
	//printf("after connected\n");
	/*if (Massive[0].iused())
		printf("|TRUE|");
	else
		printf("|FALSE|");
	if (Massive[1].iused())
		printf("|TRUE|\n");
	else
		printf("|FALSE|\n");
	*/
	/*
	delete psock;
	sConnected = Connected;
	sState = State;
	sUser = User;
	*/
	//printf("%s\n", result);
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
