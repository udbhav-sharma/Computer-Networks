#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
	int file_server,file_client;
	int pid=fork();
	
	if(pid>0)
	{
		char clientmsg[100];
		file_server=open("file_server",O_RDWR);
		while(1)
		{
			if(file_server<-1)
			{		
				printf("error in opening file file_server\n");
				exit(-1);
			}
			int size=0;
			char temp[2]={'0'};
			while(temp[0]!='/')
			{
				size=size*10+(temp[0]-'0');
				read(file_server,temp,1);
			}
			read(file_server,clientmsg,size+1);
			printf("Client msg: %s\n",clientmsg);
		}
		close(file_server);
	}
	else if(pid==0)
	{
		char usermsg[100];
		char writtenmsg[105];
		file_client=open("file_client",O_RDWR);
		while(1)
		{
			scanf("%s",usermsg);
			if(file_client<1)
			{	
				printf("error in opening file file_client\n");
				exit(-1);
			}
			int i=0,n=strlen(usermsg);
			while(n>0)
			{
				i++;
				n=n/10;
			}
			n=strlen(usermsg);
			writtenmsg[i+1]='\0';
			writtenmsg[i--]='/';
			while(n>0)
			{
				writtenmsg[i--]=(n%10)+'0';
				n=n/10;
			}
			strcat(writtenmsg,usermsg);
			write(file_client,writtenmsg,strlen(writtenmsg)+1);
		}
		close(file_client);
	}
	else
	{
		printf("Error in creating Child process\n");
		exit(-1);
	}
	exit(1);
}