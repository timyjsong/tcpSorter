server: server.o mergesort.o
	gcc mergesort.o server.o -Wall -Wextra -g -o server -lpthread

mergesort.o: mergesort.c server.h
	gcc -g -c mergesort.c server.h

server.o: server.c server.h
	gcc -g -c server.c server.h