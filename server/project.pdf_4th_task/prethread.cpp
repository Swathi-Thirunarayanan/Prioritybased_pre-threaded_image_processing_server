
extern "C"
{
#include "csapp.h"
}
#define QUEUESIZE 3

typedef struct {
	int buf[QUEUESIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);
void millisleep(int milliseconds);
void sendreceive(int connfd);
void *worker(void *);

int main(int argc, char **argv) 
{
	int listenfd, connfd, port, n, y, ii, result;
	socklen_t clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	int bytesReceived = 0;
	char recvBuff[256];
	pthread_t cons[3];
	queue * Q;
	if (argc != 2) 
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}
	port = atoi(argv[1]);
	Q = queueInit();
	if (Q ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}
	pthread_create (&cons[0], NULL, worker, Q);
	pthread_create (&cons[1], NULL, worker, Q);
	pthread_create (&cons[2], NULL, worker, Q);
	listenfd = Open_listenfd(port);
    
	while (1) 
	{
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		pthread_mutex_lock (Q->mut);
		while (Q->full) {
			printf ("producer: queue FULL.\n");
			pthread_cond_wait (Q->notFull, Q->mut);
		}
		queueAdd(Q,connfd);
		pthread_mutex_unlock (Q->mut);
		pthread_cond_signal (Q->notEmpty);
		millisleep (200);
	}
}

void *worker(void *fde)
{
	queue *fifo;
	int d;
	fifo = (queue *)fde;
	while(1)
	{
		pthread_mutex_lock (fifo->mut);
		while (fifo->empty) {
			printf ("consumer: queue EMPTY.\n");
			pthread_cond_wait (fifo->notEmpty, fifo->mut);
		}
		queueDel (fifo, &d);
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notFull);
		printf ("consumer: recieved %d.\n", d);
		Sleep(15);
		//~ doit(d);
		sendreceive(d);
    		Close(d);
		//~ if (!fork())
		 //~ { // this is the child process
			//~ close(listenfd);
			//~ /* Determine the domain name and IP address of the client */
			//~ hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,sizeof(clientaddr.sin_addr.s_addr), AF_INET);
			//~ haddrp = inet_ntoa(clientaddr.sin_addr);
			//~ printf("server connected to %s (%s)\n", hp->h_name, haddrp);
			//~ Close(connfd);
	}
}
/* $end echoserverimain */


queue *queueInit (void)
{
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);
	
	return (q);
}

void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);	
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

void queueAdd (queue *q, int in)
{
	q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
	q->empty = 0;

	return;
}

void queueDel (queue *q, int *out)
{
	*out = q->buf[q->head];

	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}

void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}
