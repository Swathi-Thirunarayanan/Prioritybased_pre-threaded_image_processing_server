+/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
extern "C"
{
#include "csapp.h"
}

void *thread(void *);

int main(int argc, char **argv) 
{
    int listenfd, *connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    int bytesReceived = 0;
    char recvBuff[256];
    pthread_t t;
    if (argc != 2) 
    {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
    port = atoi(argv[1]);
	listenfd = Open_listenfd(port);
	while (1) 
	{
		clientlen = sizeof(clientaddr);
		connfd = (int *)malloc(sizeof(int));
		*connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		Pthread_create(&t, NULL, thread, connfd);
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
		sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);

    }
}
/* $end echoserverimain */


