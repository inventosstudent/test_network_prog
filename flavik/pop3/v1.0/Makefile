all: server

server:server.o
	g++ server.o -o server -lpthread
server.o:server.cpp
	g++ -g -Wall -c server.cpp -o server.o

clean:
	rm -f *.o server
