CC = g++

CFLAGS = -c

main: stack.o utils.o
	$(CC) stack.o utils.o -o main

stack.o: stack.cpp
	$(CC) $(CFLAGS) stack.cpp

utils.o: utils.cpp
	$(CC) $(CFLAGS) utils.cpp

clean:
	rm -rf *.o main.exe