CC = g++

CFLAGS = -c

main: stack.o utils.o
	$(CC) stack.o utils.o -o main

debug: tests.o utils.o
	$(CC) tests.o utils.o -o debug

tests.o: tests.cpp
	$(CC) $(CFLAGS) tests.cpp

stack.o: stack.cpp
	$(CC) $(CFLAGS) stack.cpp

utils.o: utils.cpp
	$(CC) $(CFLAGS) utils.cpp

clean:
	rm -rf *.o main.exe debug.exe