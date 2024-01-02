
CC = g++
RESULTPRG = PrintMsg

target : main.o myLib.o
	$(CC) -o $(RESULTPRG) main.o myLib.o

main.o : main.cpp
	$(CC) -c main.cpp

myLib.o : myLib.cpp myLib.h
	$(CC) -c myLib.cpp
	
clean:
	rm *.o $(RESULTPRG)
