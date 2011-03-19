all: server.o server

server:
	g++ server.o -levent -o server 

server.o:
	g++ -g -Wall -c server.cpp -o server.o

unit_multiplex.o:
	g++ -g -Wall -c unit_multiplex.cpp -o unit_multiplex.o

unit_handler.o:
	g++ -g -Wall -c unit_handler.cpp -o unit_handler.o

unit_data.o:
	g++ -g -Wall -c unit_data.cpp -o unit_data.o

clean:
	rm -f *.o server

