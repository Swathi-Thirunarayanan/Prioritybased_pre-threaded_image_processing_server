
extern "C"
{
#include "csapp.h"
}
void sendreceive(int connfd);
void *thread(void *connfd1)
{
   
    printf("received client request\n");
    Sleep(10);
    sendreceive(*(int*)connfd1);
    Close(*(int*)connfd1);
}
