#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <algorithm>
#include <set>

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 16     // how many pending connections queue will hold
#define MAXDATASIZE 128
#define MAXDATARECV 16
#define MAXUSERS 2

using namespace std;

class Person 
{
	struct ExtStr 
	{
		string st;
		bool del;
	};
private:
	string username;
	string password;
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
		//printf("!!!!!\n");
		return 0;
	};

	string user()
	{
		return username;
	};	
	
	void user(string name)
	{
		username = name;
		return;
	};
	
			
	string pass()
	{
		return password;
	};	
	
	void pass(string name)
	{
		password = name;
		return;
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

/*	bool iused()
	{
		return used;
	};
*/
}; 

enum States 
{
	AUTHORIZATION,
	TRANSACTION,
	UPDATE
};

struct ClientInfo
{
	int sock;
	States state;
	bool connected;
	int user;
	char buf[MAXDATASIZE];
	int cntwrcmd;
};


Person Massive[3];

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
/*
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
*/
void string_handler(int &sSock, States &sState, bool &sConnected, int &sUser, char buf[])
{	
	int *psock = new int;
	*psock = sSock;


	
	bool Connected = sConnected;
	int User = sUser;
	States State = sState;
	//waiting connections
	/*
	while (1) {
		char buf[MAXDATASIZE];
		int numbytes;
		if ((numbytes = recv(*psock, buf, MAXDATASIZE - 1, 0)) == -1)  {
			perror("recieve");
			exit(1);
		}
		
		buf[numbytes] = '\0';
		*/
		char command[5], param[MAXDATASIZE-5]; 
		command[0] = '\0';
		param[0] = '\0';
		sscanf(buf,"%s %s",command,param);
	
		//printf("|%s|\n",buf);
		//printf("|%s|%s|\n",command,param);
	
	/*	char command[5];	
		strcpy(command,buf);	
		//command[5] = '\0';
		char *str = buf;
		
		//printf("!!!!!\n|");
		
		str += 5;
		//printf("||%d||%s||\n",numbytes,str);
		char str2[MAXDATASIZE];
		
		//printf("!!!!!\n");
		
		strncpy(str2,str,10);		
		
		//printf("!!!!!\n");
		
		printf("%s||\n||",command);
		printf("!!!");
	*/	

		switch (State) {
			case AUTHORIZATION:
			{
				
				if (strcmp(command, "USER") == 0) {
					
					//printf("|%s|%s|\n",command,param);

					bool flag = false;
					for (int i = 0; i < MAXUSERS; i++) {
						if (Massive[i].login(param) == 1) {
							if (User != -1) {
								Massive[User].unused();
								//printf("!!!!!!!/n");
							}
							
							User = i; 
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
					if ((User != -1)&&(!flag)) {
						char snd[] = "+OK name is valid mailbox\n";
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						} 
						State = AUTHORIZATION;
						char *usr = new char[MAXDATASIZE];
						strcpy(usr,Massive[User].user().c_str());
						printf("%s logined\n", usr);
						delete [] usr;
					}
					else
					{
						char snd[] = "-ERR wrong username\n";
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}
					}
				} 
				else

				if (strcmp(command, "PASS") == 0) {
					
					//printf("!!!!\n");
					
					if (User != -1) {
						if (Massive[User].pass() == param) {
							char snd[] = "+OK maildrop locked and ready\n";
							if (send(*psock, snd, strlen(snd), 0) == -1) {
								perror("send");
							} 
							State = TRANSACTION;
							char *usr = new char[MAXDATASIZE];
							strcpy(usr,Massive[User].user().c_str());
							printf("%s authorizated\n",usr);
							delete [] usr;
						}
						else
						{	
							char snd[] = "-ERR invalid password\n";
							if (send(*psock, snd, strlen(snd), 0) == -1) {
								perror("send");
							} 
						}
					}		
					else
					{
						char snd[] = "-ERR client is not identified\n";
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}
					}
				}
 				else
	
				if (strcmp(command, "QUIT") == 0) {
					if (strcmp(param,"") == 0) {
						
						printf("client disconnect\n");
						Connected = false;
						
						char snd[] = "+OK dewey POP3 server\n";
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}	
						//close(*psock);
						State = UPDATE;
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
				{	
					char snd[] = "-ERR invalid command\n";
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
				}
				
				//printf("break switch\n");
				break;
			}
			
			case TRANSACTION:
			{
				
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
				
				if (strcmp(command, "LIST") == 0) {
					
					//printf("|%s|%s|\n",command,param);
						
					if (strcmp(param,"") == 0) {
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
					}
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
					}
				} 
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
				} 
				else

				if (strcmp(command, "QUIT") == 0) {
					if (strcmp(param,"") == 0) {

						printf("client disconnect\n");
						Connected = false;
						
						char snd[] = "+OK dewey POP3 server\n";
						if (send(*psock, snd, strlen(snd), 0) == -1) {
							perror("send");
						}	
						//close(*psock);
						State = UPDATE;
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

				{	
					char snd[] = "-ERR invalid command\n";
					if (send(*psock, snd, strlen(snd), 0) == -1) {
						perror("send");
					}
				}
				
				break;		
			}
			case UPDATE:
			{
				break;
			}
		}	
		
		if (State == UPDATE) {
			if (User != -1) {
				Massive[User].updatemailbox();
				Massive[User].unused();
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

	delete psock;
	sConnected = Connected;
	sState = State;
	sUser = User;

	//pthread_exit(NULL);
}



int main(void)
{	
	Massive[0].user("user0");
	Massive[0].pass("password0");
	Massive[0].addmessage("message0 user0");
	Massive[0].addmessage("petrovich sprava ot menya");
	
	Massive[1].user("user1");
	Massive[1].pass("password1");
	Massive[1].addmessage("message0user1");
	Massive[1].addmessage("message1user1");
	Massive[1].addmessage("message2user1");


    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

	vector<ClientInfo> clients;
	clients.clear();

/*
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
*/

    printf("pop3 server: waiting for connections...\n");

    while(1) {  // main accept() loop
        //sin_size = sizeof their_addr;
	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(sockfd, &readset);

	int mx2 = -999999999; 
	for (vector<ClientInfo>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (mx2 < it->sock) 
			mx2 = it->sock;
		FD_SET(it->sock, &readset);
	}
	
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int mx = max(sockfd, mx2/**max_element((clients.begin())->sock, (clients.end())->sock)*/);
	if (select(mx+1, &readset, NULL, NULL, &timeout) < 0) {
		perror("select");
		exit(3);
	}

	if (FD_ISSET(sockfd, &readset))
	{
		int new_fd = accept(sockfd, NULL, NULL);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		fcntl(new_fd, F_SETFL, O_NONBLOCK);

		char snd[] = "+OK POP3 server ready\n";
		if (send(new_fd, snd, strlen(snd), 0) == -1)
			perror("send");

		ClientInfo tmp;
		tmp.sock = new_fd;
		tmp.connected = true;
		tmp.state = AUTHORIZATION;
		tmp.user = -1;
		tmp.buf[0] = '\0';
		tmp.cntwrcmd = 0;
		
		clients.push_back(tmp);
	}

	for (vector<ClientInfo>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (FD_ISSET(it->sock, &readset))
		{
			//char buffer[MAXDATASIZE];
			int l = strlen(it->buf);
			char *w = &(it->buf[l]);

			bool end_recv = false;
			

			if ((l+MAXDATARECV) > MAXDATASIZE) {
				it->buf[0] = '\0';
				char snd[] = "-ERR invalid command\n";
				if (send(it->sock, snd, strlen(snd), 0) == -1) {
					perror("send");
				}
				
				it->cntwrcmd++;
				
				printf("\n\n%d\n\n", it->cntwrcmd);


				if (it->cntwrcmd > 7) {	
					close(it->sock);
					vector<ClientInfo>::iterator w = it;
					it--;
					clients.erase(w);
				}
			}
			else
			{
				int numbytes = recv(it->sock, w, MAXDATARECV, 0);
				if (numbytes == -1) {
					perror("receive");
					continue;
				}

				printf("|%s|\n",it->buf);

				for (int i=0; i < numbytes-1; i++) {
					if ( ( (*(w+i) == '\r')&&(*(w+i+1) == '\n')) || ( (*(w+i) == '\n')&&(*(w+i+1) == '\r')) ) {
						*(w+i) = '\0';
						end_recv = true;
						break;
					}
				}
				
				printf("|%s|\n", it->buf);

				if (end_recv) {	
					string_handler(it->sock, it->state, it->connected, it->user, it->buf);
					it->buf[0] = '\0';

					if (it->connected == false){
						close(it->sock);
						vector<ClientInfo>::iterator w = it;
						it--;
						clients.erase(w);
					}
				}
				else
				{
					*(w+numbytes) = '\0';
				}
				
			}

			//ClientInfo tmp;
			//tmp = *it;
			


		}
	}
	
    }

    return 0;
}
