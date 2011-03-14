all:bot test

bot:bot.o
	g++ bot.o -o bot
bot.o:bot.cpp
	g++ -g -Wall -c bot.cpp -o bot.o
clean:
	rm -f *.o bot
test:test.o
	g++ test.o -o test
test.o:test.cpp
	g++ -g -Wall -c test.cpp -o test.o
