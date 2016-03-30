/*
Simulation of Stop and Wait Protocol
Developed by
E.T JabaJasphin,
Assistant Professor,
Saveetha Engineering College.
E-Mail: jabajasphin@gmail.com
*/

#include<stdio.h>
#include<string.h>
#include <netprotolib.h>
enum frametype {DATA_FRAME,ACK_FRAME};
struct dataframe
{
	enum frametype frame;
	int framenumber;
	char data[200];

};
int framenumber;
struct dataframe df,af;

int main()
{
int remoteport,localport,direction;
float errorrate;
char remotehost[100],filename[50],word[50];
FILE *file;


	printf("Stop and Wait Protocol!!!\n");
	printf("Enter Remotehost remoteport localport errorrate:");
	scanf("%s%d%d%f",remotehost,&remoteport,&localport,&errorrate);
	
	if(initprotocol(remotehost,remoteport,localport,errorrate)<0)
	{
		printf("Initialization failed...\n");
		return -1;
	}

	printf("1.Send \n2.Receive\n");
	scanf("%d",&direction);
	if(direction==1)
	{
		printf("Enter the name of the file to be sent:");
		scanf("%s",filename);
		file=fopen(filename,"r");
		if(file==NULL)
		{	
			printf("File open failed...");
			return -1;
		}
		framenumber=0;
		while(1)
		{
			fscanf(file,"%s",word);
			if(feof(file))
				break;
			df.frame=DATA_FRAME;
			df.framenumber=framenumber;
			strcpy(df.data,word);
			printf("Sending Frame %d\n",df.framenumber);
			while(1)
			{
				printf("Data:%s\n",df.data);
				sendframe(&df,sizeof(df));
				if(receiveframe(&af,sizeof(af),NULL,1)==0)
				{
					if(af.framenumber==df.framenumber)
						break;
				}
				printf("\t\tResending frame %d:\n",df.framenumber);
			}
			framenumber=1-framenumber;
		}
	}else
	{
		printf("Waiting for frame...\n");
		framenumber=1;
		af.frame=ACK_FRAME;
		while(1)
		{
			if(receiveframe(&df,sizeof(df),NULL,1)==0)
			{
				if(df.framenumber==1-framenumber&& df.frame==DATA_FRAME)
				{
					af.framenumber=df.framenumber;
					sendframe(&af,sizeof(af));	
					printf("Received frame %d\n",df.framenumber);
					printf("Data:%s\n",df.data);
					framenumber=df.framenumber;
				}
				else if(df.frame==DATA_FRAME)
				{
					af.framenumber=df.framenumber;
					sendframe(&af,sizeof(af));					
				}else
				{
					printf("\t\tInvalid frame %d\n",df.framenumber);
				}
			}
		}
	}
return 0;
}
