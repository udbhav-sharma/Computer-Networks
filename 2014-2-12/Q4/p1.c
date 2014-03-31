#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

main()
{
	int sfd,sfd2,portno,n;
	char buf[256];
	struct sockaddr_in serv_addr,new_serv_addr;
	struct hostent *server;

	sfd=socket(AF_INET,SOCK_STREAM,0);

	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=8000;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    char *msg="Server u wanna connect:\t";
    write(0,msg,strlen(msg));
    n=read(0,buf,255);
    if(n<0)
    {
    	printf("ERROR in reading input\n");
    	exit(0);
    }
    
    write(sfd,buf,strlen(buf)+1);
    sleep(3);
    write(sfd,"p1",3);
    read(sfd,buf,255);
    
    if(strcmp(buf,"No")==0)
    {
    	strcpy(buf,"Cant connect");
    	write(1,buf,strlen(buf)+1);
    	exit(0);
    }
    else
    {
    	int i;
    	new_serv_addr.sin_addr.s_addr=inet_addr(buf);
    	
    	write(1,"Waiting for reading\n",20);
    	read(sfd,buf,strlen(buf)+1);
    	
    	close(sfd);
    	sfd=socket(AF_INET,SOCK_STREAM,0);

    	write(1,"Waiting after writing\n",23);
    	portno=0;
    	for(i=0;i<strlen(buf);i++)
    		portno=portno*10+(buf[i]-'0');

    	//portno=11112;
    	
    	new_serv_addr.sin_family=AF_INET;
    	new_serv_addr.sin_port=htons(portno);
    	write(1,"Waiting for connect\n",23);
    	if((n=connect(sfd,(struct sockaddr *)&new_serv_addr, sizeof(new_serv_addr)))<0) 
    	{
    		perror("ERROR connecting");
    		exit(0);
    	}
    	write(1,"Connection done\n",17);
    }
    close(sfd);
    exit(1);
}