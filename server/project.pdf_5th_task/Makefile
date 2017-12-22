CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: PriorWorker

PriorWorker: PriorityWorker.cpp csapp.o
	g++ -std=c++11 PriorityWorker.cpp sendreceive.cpp greyscale_CV.cpp `pkg-config --libs --cflags opencv` csapp.o -o PriorWorker -lpthread

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm PriorWorker
	


