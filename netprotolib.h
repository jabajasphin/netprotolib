#ifndef _NETPROTOLIB_
#define _NETPROTOLIB_
/*
Library functions to simulate protocol implementation
Developed by
E.T JabaJasphin,
Assistant Professor,
Saveetha Engineering College.
E-Mail: jabajasphin@gmail.com
*/
int initprotocol(char *host,int remortport,int localport,double errorrate);
int sendframe(void *data,int length);
int receiveframe(void *data,int datalength,int *receivedlength,long waitingtimems);
#define PRO_BUFFER_LENGTH	20
#define PRO_DATA_LENGTH		1024

#endif
