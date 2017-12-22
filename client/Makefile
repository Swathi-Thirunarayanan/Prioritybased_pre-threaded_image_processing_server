CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: client 

client: echoclient.cpp csapp.o
	g++ -std=c++11 echoclient.cpp `pkg-config --libs --cflags opencv` csapp.o -o client -lpthread
csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm -f *.o client *~
	
