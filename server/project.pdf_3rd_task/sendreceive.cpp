/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
extern "C"
{
#include "csapp.h"
}
#include <stdio.h>
#include <iostream>
//char imageRcvd = 1;
int greyscale_CV();

void sendreceive(int connfd) 
{
	size_t n;
	int bytesReceived = 0; 
	char recvBuff[256]; 
	rio_t rio;

	Rio_readinitb(&rio, connfd);
	int rx_fsize;
	
	
	Rio_readnb(&rio,&rx_fsize,sizeof(rx_fsize));
	
	printf("Size of file %d received\n",rx_fsize );
	FILE *fp;
	fp = fopen("image.png", "wb");
	if(NULL == fp)
	{
		printf("Error opening file");
		return;
	}
	
	fseek(fp, 0, SEEK_SET);
	int recData = 256;
	
	while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && rx_fsize>0)
	{
		printf("Bytes received %d\n",bytesReceived);    
		
		fwrite(recvBuff, 1,bytesReceived,fp);
		
		rx_fsize -= bytesReceived;
		printf("remaining bytes :%d to be received\n",rx_fsize);
		if (rx_fsize<256)	//if remaining bytes is less than 256, read only remaining bytes of data
        	recData = rx_fsize;
	}
	fclose(fp);
	printf("File received from the client\n");
	
	greyscale_CV();
		FILE *fp2;
		fp2 = fopen("Gray_Image.png", "rb");
		if(NULL == fp2)
		{
			printf("Error opening file");
			return;
		}
		fseek(fp2, 0, SEEK_END);
		int tx_size = ftell(fp2);
		fseek(fp2, 0, SEEK_SET);
		printf("size of file sent = %d\n",tx_size);

		//send the size of image
		Rio_writen(connfd, &tx_size, sizeof(tx_size));
		fseek(fp2, 0, SEEK_SET);

		//sending back image file to client
		while(1)
		{
			/* First read file in chunks of 256 bytes */
			
			unsigned char buff[256];

			int nread = fread(buff,1,256,fp2);
			printf("File read :%d bytes\n", nread);        

			/* If read was success, send data. */
			if(nread > 0)
			{
				printf("Sending back to the client\n");
            
				Rio_writen(connfd, buff, nread);
			}

			/*
			* There is something tricky going on with read .. 
			* Either there was error, or we reached end of file.
			*/
			if (nread < (256))
			{
				if (feof(fp2))
					printf("End of file\n");
				if (ferror(fp2))
					printf("Error reading\n");
				break;
			}
		}
		fclose(fp2);
	
	
}

