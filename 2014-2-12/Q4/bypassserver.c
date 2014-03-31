#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int cmp(char s1[255],char s2[255])
{
	int i;
	for(i=0;i<strlen(s2);i++)
		if(s2[i]!=s1[i])
			return 0;
	return 1;
}

main()
{
	int i,sfd,sfd2;
	int portno,portno2;
	struct sockaddr_in server_addr,new_server_addr;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	
	sfd2=socket(AF_INET,SOCK_STREAM,0);

	if(sfd<0 || sfd2<0)
	{
		perror("Socket error\n");
		exit(0);
	}

	portno=8000;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);

	if(bind(sfd,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		perror("ERROR on binding");
		exit(0);
	}

	listen(sfd,5);

	while(1)
	{
		char buf[255],buf2[255];
		struct sockaddr_in cli_addr;
		int clilen=sizeof(cli_addr);
		int nsfd=accept(sfd,(struct sockaddr *)&cli_addr,&clilen);

		read(nsfd,buf,strlen(buf)+1);
		write(1,"Read from client\n",18);

		if(cmp(buf,"p5"))
		{
			strcpy(buf2,"11112");
			portno2=12112;
		}
		else if(cmp(buf,"p6"))
		{
			strcpy(buf2,"11122");
			portno2=12122;
		}
		else if(cmp(buf,"p7"))
		{
			strcpy(buf2,"11132");
			portno2=12132;
		}
		else if(cmp(buf,"p8"))
		{
			strcpy(buf2,"11142");
			portno2=12142;
		}

		new_server_addr.sin_family = AF_INET;
		new_server_addr.sin_addr.s_addr =inet_addr("127.0.0.1");
		new_server_addr.sin_port = htons(portno2);

		if(connect(sfd2,(struct sockaddr *) &new_server_addr, sizeof(new_server_addr)) < 0)
		{
			perror("ERROR on connect");
			exit(0);
		}

		read(nsfd,buf,255);
		write(1,"Read from client\n",18);

		write(sfd2,buf,strlen(buf)+1);
		write(1,"Written to another server\n",26);
		read(sfd2,buf,255);

		if(strcmp(buf,"No")==0)
		{
			write(nsfd,buf,strlen(buf)+1);
		}
		else
		{
			write(nsfd,"127.0.0.1",10);
			sleep(3);
			write(nsfd,buf2,strlen(buf2)+1);
		}
		write(1,"Writing done\n",14);
	}
	exit(1);
}