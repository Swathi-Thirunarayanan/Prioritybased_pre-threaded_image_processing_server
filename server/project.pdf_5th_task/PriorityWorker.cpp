
extern "C"
{
#include "csapp.h"
}

#define QUEUESIZE 	6
#define DELAY 		3000	

// Thread 
pthread_t w1;
pthread_t w2;
pthread_t w3;


typedef struct {
	int buf[QUEUESIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;

//Queue functions
queue *queueInit (void);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);
void millisleep(int milliseconds);

void *consumer(void *);
void sendreceive(int connfd);


int main(int argc, char **argv) 
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;

    int bytesReceived = 0;
    char recvBuff[256];
    
    if (argc != 2) 
    {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
    port = atoi(argv[1]);
    queue *fifo;
    fifo = queueInit ();
	if (fifo ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}
	struct sched_param my_param;
	pthread_attr_t worker_attr1;
	pthread_attr_t worker_attr2;
	pthread_attr_t worker_attr3;
	int i, min_priority=0, policy;
	
	/* MAIN-THREAD WITH HIGH PRIORITY */
	my_param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	min_priority=my_param.sched_priority;
	my_param.sched_priority= my_param.sched_priority + 3;

	pthread_setschedparam(pthread_self(), SCHED_RR, &my_param);
	pthread_getschedparam (pthread_self(), &policy, &my_param);
	printf("Server priority=%d\n",my_param.sched_priority);


	/* SCHEDULING POLICY AND PRIORITY FOR OTHER THREADS */
	pthread_attr_init(&worker_attr1);
	pthread_attr_init(&worker_attr2);
	pthread_attr_init(&worker_attr3);
	pthread_attr_setinheritsched(&worker_attr1, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setinheritsched(&worker_attr2, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setinheritsched(&worker_attr3, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&worker_attr1, SCHED_FIFO);
	pthread_attr_setschedpolicy(&worker_attr2, SCHED_FIFO);
	pthread_attr_setschedpolicy(&worker_attr3, SCHED_FIFO);

	my_param.sched_priority = min_priority + 1;
	pthread_attr_setschedparam(&worker_attr1, &my_param);
	my_param.sched_priority = min_priority + 1;
	pthread_attr_setschedparam(&worker_attr2, &my_param);
	my_param.sched_priority = min_priority + 1;
	pthread_attr_setschedparam(&worker_attr3, &my_param);
    
	Pthread_create (&w1, &worker_attr1, consumer, fifo);
	Pthread_create (&w2, &worker_attr2, consumer, fifo);
	Pthread_create (&w3, &worker_attr3, consumer, fifo);
	
	//~ pthread_barrier_init(&mybarrier,NULL, PRIO_GROUP*3);
	listenfd = Open_listenfd(port);
	clientlen = sizeof(clientaddr);
	while (1) 
	{
		
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		pthread_mutex_lock (fifo->mut);
		queueAdd (fifo, connfd);
		printf("producer: added %dth connfd to the buffer\n",connfd);
		while (fifo->full) {
			printf ("producer: queue FULL.\n");
			pthread_cond_wait (fifo->notFull, fifo->mut);
		}
		
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notEmpty);
    }
}
/* $end echoserverimain */

void *consumer(void *fileDescriptor){
	queue *fifo;
	fifo=(queue *)fileDescriptor;
	int fd,d;
	pthread_t thread_id = pthread_self();
	struct sched_param param;
	int priority, policy, ret;
	ret = pthread_getschedparam (thread_id, &policy, &param);
		while(1){
		pthread_mutex_lock (fifo->mut);
		while (fifo->empty) {
			printf ("consumer: queue EMPTY.\n");
			pthread_cond_wait (fifo->notEmpty, fifo->mut);
		}
		queueDel (fifo, &fd);
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notFull);
		printf ("consumer: recieved %dth connfd from the buffer\n", fd);
		priority = param.sched_priority;	
		printf("Priority of worker : %d \n", priority);
		for(d=0;d<DELAY;d++){
			millisleep(10);
		}
		sendreceive(fd);
		printf("consumer: Served %dth connfd \n",fd);
		Close(fd);
		
	}

}


//queue function definitions
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

