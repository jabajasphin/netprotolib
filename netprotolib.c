/*
Library functions to simulate protocol implementation
Developed by
E.T JabaJasphin,
Assistant Professor,
Saveetha Engineering College.
E-Mail: jabajasphin@gmail.com
*/

#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include "netprotolib.h"

static int prosendsocket,prorecvsocket;/*socket for sending and receiving purpose*/
static int proremoteport;
static int proinitstatus=-1;
static struct hostent *proremotehost;  /*to store host name of remote machine*/
static double proerrorrate;
pthread_mutex_t prolock;

static struct sockaddr_in prolocaladdress,proremoteaddress; 
/* to store IP address of local and remote machines*/

static char probuffer[PRO_BUFFER_LENGTH][PRO_DATA_LENGTH];
static int probufferindexin,probufferindexout;

void * receiveframethread(void *arg)
{
char tempdata[PRO_DATA_LENGTH];
socklen_t taddresslength ; 
struct sockaddr_in tremoteaddress;
int receivedcount;

	probufferindexin=0;
	probufferindexout=0;
	while(1)
	{	
	memset((char *)&tremoteaddress, 0, sizeof(tremoteaddress));
	receivedcount = recvfrom(prorecvsocket, tempdata, sizeof(tempdata), 0, (struct sockaddr *)&tremoteaddress, &taddresslength);
	
		if(rand()<proerrorrate*RAND_MAX)
			continue;
		if(receivedcount>0)
		{
			pthread_mutex_lock(&prolock);
			if(((probufferindexin+1)%PRO_BUFFER_LENGTH)!=probufferindexout)
			{
			memcpy(&(probuffer[probufferindexin][0]),tempdata,sizeof(tempdata));
			probufferindexin=(probufferindexin+1)%PRO_BUFFER_LENGTH;
			
			}else
			{
				printf("Receive buffer is full.");
			}
			pthread_mutex_unlock(&prolock);
		}		
	}
}

int initprotocol(char *host,int remoteport,int localport,double errorrate)
{
int tres;
pthread_t tthread;
void *tthreadresult;
	if(proinitstatus==1)
	{
		printf("initprotocol is already executed.\n");
		return -1;
	}

	if (pthread_mutex_init(&prolock, NULL) != 0)
	{
        	printf("Mutex init failed\n");
        	return -1;
    	}
	proerrorrate=errorrate;
	/*To create a datagram(UDP) socket*/
	prosendsocket=socket(AF_INET,SOCK_DGRAM,0);
	if(prosendsocket<0)
	{
		printf("Cannot create socket...\n");
		return -1;
	}

	/*To initialize the localaddress with zero*/
	memset((char *)&prolocaladdress, 0, sizeof(prolocaladdress));
	prolocaladdress.sin_family = AF_INET;  /*To use IP protocol */
	prolocaladdress.sin_addr.s_addr = htonl(INADDR_ANY); /*To use any of the local IP*/
	prolocaladdress.sin_port = htons(0); /*To use any port*/

	/*To bind the sending socket with local port*/
	if (bind(prosendsocket, (struct sockaddr *)&prolocaladdress, sizeof(prolocaladdress)) < 0) {
		printf("Socket binding failed\n");
		return -1;
	}

	/*to convert host to IP*/
	proremotehost = gethostbyname(host);
	if(!proremotehost)
	{
		printf("Invalid remote IP address...\n");
		return -1;
	}
	/*To initialize the remoteaddress with zero*/
	memset((char *)&proremoteaddress, 0, sizeof(proremoteaddress));
	proremoteaddress.sin_family = AF_INET; /*To use IP protocol */
	proremoteaddress.sin_port = htons(remoteport); /*To use a specific port*/

	/*To copy the IP address of the remote machine to remoteaddress*/
	memcpy((void *)&proremoteaddress.sin_addr, proremotehost->h_addr_list[0], proremotehost->h_length);
	
	/*To create a datagram(UDP) socket*/
	prorecvsocket=socket(AF_INET,SOCK_DGRAM,0);
	if(prorecvsocket<0)
	{
		printf("Cannot create socket...\n");
		return -1;
	}

	/*To initialize the localaddress with zero*/
	memset((char *)&prolocaladdress, 0, sizeof(prolocaladdress));
	prolocaladdress.sin_family = AF_INET;/*To use IP protocol */
	prolocaladdress.sin_addr.s_addr = htonl(INADDR_ANY);/*To use any of the local IP*/
	prolocaladdress.sin_port = htons(localport);/*To use a specific port*/

	/*To bind the socket with local port*/
	if (bind(prorecvsocket, (struct sockaddr *)&prolocaladdress, sizeof(prolocaladdress)) < 0) 
	{
		printf("Socket binding failed\n");
		return -1;
	}
	proinitstatus=1;

	tres=pthread_create(&tthread,NULL,receiveframethread,(void*)NULL);
	if(tres!=0)
	{
		printf("Thread creation failed.\n");
		return -1;
	}
return 0;
}

int sendframe(void *data,int length)
{
	if(proinitstatus!=1)
	{
		printf("initprotocol is not executed.\n");
		return -1;
	}

	if(rand()<proerrorrate*RAND_MAX)
		return 0;

	/*To send the data to remote Machine*/
	if (sendto(prosendsocket, data, length, 0, (struct sockaddr *)&proremoteaddress, sizeof(proremoteaddress)) < 0) 
	{
		printf("Sending failed ....\n");
		return -1;
	}

return 0;
}


int receiveframe(void *data,int datalength,int *receivedlength,long waitingtimems)
{
	if(proinitstatus!=1)
	{
		printf("initprotocol is not executed.\n");
		return -1;
	}

	pthread_mutex_lock(&prolock);
	if(probufferindexin!=probufferindexout)
	{
		memcpy(data,&(probuffer[probufferindexout][0]),datalength);
		probufferindexout=(probufferindexout+1)%PRO_BUFFER_LENGTH;
		pthread_mutex_unlock(&prolock);
		return 0;
	}
	pthread_mutex_unlock(&prolock);
	sleep(waitingtimems);
	pthread_mutex_lock(&prolock);
	if(probufferindexin!=probufferindexout)
	{
		memcpy(data,&(probuffer[probufferindexout][0]),datalength);
		probufferindexout=(probufferindexout+1)%PRO_BUFFER_LENGTH;
		pthread_mutex_unlock(&prolock);
		return 0;
	}
	pthread_mutex_unlock(&prolock);
return  -1;
}
