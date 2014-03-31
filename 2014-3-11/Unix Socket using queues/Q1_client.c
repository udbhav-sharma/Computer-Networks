#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
	int sfd,usfd,portno=11111,n,clilen;

    struct sockaddr_in serv_addr,cli_addr; 
	struct sockaddr_un serv_addr_unix;

    clilen=sizeof(serv_addr);

	usfd=socket(AF_UNIX,SOCK_STREAM,0);
	if (usfd < 0) 
    {
        perror("ERROR in opening unix socket\n");
        exit(0);
    }

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd<0)
    {
        perror("Error in opening socket\n");
        exit(0);
    }

    serv_addr_unix.sun_family=AF_UNIX;
    strcpy(serv_addr_unix.sun_path,"/tmp/temp.txt");

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(portno);

    if((n=connect(usfd,(struct sockaddr *)&serv_addr_unix, sizeof(serv_addr_unix)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }

    if((n=bind(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)))<0)
    {
        perror("Error in binding\n");
        exit(0);
    }

    listen(sfd,5);
    int nsfd=accept(sfd,(struct sockaddr *)&cli_addr,&clilen);

    n=send(nsfd,"This is from Q1_client",strlen("This is from Q1_client")+1,0);
    if(n<0)
    {
        perror("Error in sending msg to client\n");
        exit(0);
    }

    printf("Passing nsfd\n");
    fflush(stdout);

    char buf[1];
    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char cms[CMSG_SPACE(sizeof(int))];

    buf[0] = 0;
    iov.iov_base = buf;
    iov.iov_len = 1;

    memset(&msg, 0, sizeof msg);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = (caddr_t)cms;
    msg.msg_controllen = CMSG_LEN(sizeof(int));

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    memmove(CMSG_DATA(cmsg), &nsfd, sizeof(int));
    n=sendmsg(usfd, &msg, 0);

    printf("File descriptor passed \n");
    fflush(stdout);

    close(usfd);
    close(nsfd);
    close(sfd);
    
    exit(1);
}