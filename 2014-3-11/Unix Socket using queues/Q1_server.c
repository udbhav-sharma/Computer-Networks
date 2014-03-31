#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
    int usfd,nusfd,clilen,n;
    struct sockaddr_un serv_addr, cli_addr;

    usfd=socket(AF_UNIX,SOCK_STREAM,0);
    if (usfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path,"/tmp/temp.txt");
    unlink("/tmp/temp.txt");

    if (bind(usfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(usfd,5);
    clilen = sizeof(cli_addr);
    
    nusfd = accept(usfd,(struct sockaddr *)&cli_addr,&clilen);
    if(nusfd < 0) 
    {
        perror("ERROR on accept");
        exit(0);
    }

    char buf[1];
    int nsfd;
    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char cms[CMSG_SPACE(sizeof(int))];

    iov.iov_base = buf;
    iov.iov_len = 1;

    memset(&msg, 0, sizeof msg);
    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    msg.msg_control = (caddr_t)cms;
    msg.msg_controllen = sizeof cms;
    
    if(recvmsg(nusfd, &msg, 0)<0)
        printf("recv error\n");
    
    cmsg=CMSG_FIRSTHDR(&msg);
    
    memmove(&nsfd, CMSG_DATA(cmsg), sizeof(int));

    printf("Q1_Client send fd: %d\n",nsfd);
    fflush(stdout);
    
    n=send(nsfd,"This is from Q1_server",strlen("This is from Q1_server")+1,0);
    if(n<0)
    {
        perror("Error in sending msg to client\n");
        exit(0);
    }

    printf("Message send to client\n");
    fflush(stdout);

    close(nsfd);
    close(nusfd);
    close(usfd);

    exit(1);
}