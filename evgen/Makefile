all:server client server2 test server3
server:server.o
	g++ server.o -o server
server.o:server.cpp
	g++ -g -Wall -c server.cpp -o server.o

client:client.o
	g++ client.o -o client
client.o:client.cpp
	g++ -g -Wall -c client.cpp -o client.o

server2:server2.o
	g++ server2.o -o server2 -lpthread
server2.o:server2.cpp
	g++ -g -Wall -c server2.cpp -o server2.o

test:test.o
	g++ test.o -o test -lpthread
test.o:test.cpp
	g++ -g -Wall -c test.cpp -o test.o

server3:server3.o
	g++ server3.o -o server3 -lpthread
server3.o:server3.cpp
	g++ -g -Wall -c server3.cpp -o server3.o
