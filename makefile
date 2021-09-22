CCFLAGS = -Wall
CC = g++ -std=c++11

rolie : rolie.o
	$(CC) rolie.o -o rolie  -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lGL

rolie.o : rolie.cpp
	$(CC) -c rolie.cpp

.PHONY : clean
clean:
	rm -f rolie rolie.o core *~