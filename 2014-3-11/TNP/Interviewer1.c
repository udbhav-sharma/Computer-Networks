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
	int usfd,portno,n;
	char buf[256];
	struct sockaddr_un serv_addr;

	usfd=socket(AF_UNIX,SOCK_STREAM,0);
	if (usfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    serv_addr.sun_family=AF_UNIX;
    strcpy(serv_addr.sun_path,"/tmp/temp_AT");

    if((n=connect(usfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }

    while(1)
    {
        send(usfd,"Interviewer 1 is free",strlen("Interviewer 1 is free"),0);

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

        if(recvmsg(usfd, &msg, 0)<0)
            printf("recv error\n");

        cmsg=CMSG_FIRSTHDR(&msg);

        memmove(&nsfd, CMSG_DATA(cmsg), sizeof(int));

        printf("AT send nsfd: %d\n",nsfd);
        fflush(stdout);

        send(nsfd,"Interview going on with interviewer 1",
            strlen("Interview going on with interviewer 1"),0);

        printf("Interview done\n");
        fflush(stdout);

    }
    
    close(usfd);
    exit(1);
}