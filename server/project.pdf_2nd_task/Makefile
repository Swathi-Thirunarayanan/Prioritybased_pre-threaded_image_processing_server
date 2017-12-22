CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: server2

server2: server.cpp csapp.o
	g++ -std=c++11 server.cpp threadfunc.cpp sendreceive.cpp greyscale_CV.cpp `pkg-config --libs --cflags opencv` csapp.o -o server2 -lpthread

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm server2
	


