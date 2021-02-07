.default: all
 
all: azul
 
clean:
	rm -f azul *.o
 
azul: main.o Game.o Player.o LinkedList.o SaveAndLoad.o AI.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^
 
%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -g -O -c $^