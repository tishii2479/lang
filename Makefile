CC=g++
CFLAGS=-std=c++17 -Wall

main: main.cpp
	CC -o main main.cpp
test: main
	./test.sh
clean:
	rm -f main *.o *~ tmp*

.PHONY: test clean