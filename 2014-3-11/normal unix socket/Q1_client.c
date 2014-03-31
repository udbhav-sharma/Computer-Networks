#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
	int sfd,portno,n;
	char buf[256];
	struct sockaddr_un serv_addr;

	sfd=socket(AF_UNIX,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    serv_addr.sun_family=AF_UNIX;
    strcpy(serv_addr.sun_path,"/tmp/temp.txt");

    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    printf("Please enter the message: \t");
    fflush(stdout);

    char msg[255];
    scanf("%s",msg);

    send(sfd,msg,strlen(msg)+1,0);
    
    recv(sfd,buf,255,0);

    printf("%s\n",buf);
    fflush(stdout);

    close(sfd);
    
    exit(1);
}