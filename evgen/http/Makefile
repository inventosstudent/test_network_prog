all:http_serv
http_serv:http_serv.o
	g++ http_serv.o -o http_serv -levent
http_serv.o:http_serv.cpp
	g++ -g -Wall -c http_serv.cpp -o http_serv.o
