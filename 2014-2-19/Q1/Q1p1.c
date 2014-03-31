#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>

int sfd_tcp,sfd_udp;
void closesfd(int sig)
{
    close(sfd_udp);
    close(sfd_tcp);
    write(1,"Byee\n",5);
    exit(0);
}

main()
{
	(void )signal(SIGINT,closesfd);

    sfd_tcp=socket(AF_INET,SOCK_STREAM,0);
	sfd_udp=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	
    if(sfd_tcp<0)
	{
		perror("Error in creating socket stream\n");	
	}
    if(sfd_udp<0)
    {
        perror("Error in creating socket udp\n");
    }

	struct sockaddr_in serv_addr_udp,serv_addr_tcp,cli_addr_tcp,cli_addr_udp;
	int cli_len_tcp=sizeof(cli_addr_tcp);
    int cli_len_udp=sizeof(cli_addr_udp);

	int portno_udp=8000;
	serv_addr_udp.sin_family = AF_INET;
    serv_addr_udp.sin_addr.s_addr = INADDR_ANY;
    serv_addr_udp.sin_port = htons(portno_udp);

    int portno_tcp=9000;
    serv_addr_tcp.sin_family = AF_INET;
    serv_addr_tcp.sin_addr.s_addr = INADDR_ANY;
    serv_addr_tcp.sin_port = htons(portno_tcp);

    if(bind(sfd_tcp,(struct sockaddr *) &serv_addr_tcp, sizeof(serv_addr_tcp)) < 0)
    {
        perror("ERROR on binding in sfd_tcp");
        exit(0);
    }

    if(bind(sfd_udp,(struct sockaddr *) &serv_addr_udp, sizeof(serv_addr_udp)) < 0)
    {
        perror("ERROR on binding in sfd_udp");
        exit(0);
    }

    listen(sfd_tcp,1);

    int portno2=9010;
    cli_addr_udp.sin_family = AF_INET;
    cli_addr_udp.sin_addr.s_addr = inet_addr("127.0.0.1");
    cli_addr_udp.sin_port = htons(portno2);
    
    fd_set r_fds;
    FD_ZERO(&r_fds);
    int max_fd=0,i,res;
    
    FD_SET(sfd_tcp,&r_fds);
    FD_SET(sfd_udp,&r_fds);

    if(sfd_tcp>sfd_udp)
        max_fd=sfd_tcp;
    else
        max_fd=sfd_udp;
    struct timeval tv;
    tv.tv_usec=0;
    tv.tv_sec=5;
    while(1)
    {
        FD_SET(sfd_tcp,&r_fds);
        FD_SET(sfd_udp,&r_fds);
        res = select(max_fd+1, &r_fds, NULL, NULL,&tv);
        if(res>0)
        {
            if(FD_ISSET(sfd_tcp,&r_fds))
            {
                int nsfd=accept(sfd_tcp,(struct sockaddr *)&cli_addr_tcp,&cli_len_tcp);
                write(1,"Handling TCP service\n",strlen("Handling TCP service\n"));
                write(nsfd,"U got connected successfully through tcp socket",strlen("U got connected successfully through tcp socket"));
                close(nsfd);
            }
            else
            {
                char buf[255];
                write(1,"Handling UDP service\n",strlen("Handling UDP service\n"));
                recvfrom(sfd_udp,buf,255,0,(struct sockaddr *) &cli_addr_udp,&cli_len_udp);
                write(1,buf,strlen(buf));
                write(1,"\n",1);
                sendto(sfd_udp,"Message received successfully through udp socket",
                    strlen("Message received successfully through udp socket"),0,(struct sockaddr *) &cli_addr_udp,cli_len_udp);
            }
        }
    }
    close(sfd_udp);
    close(sfd_tcp);
    exit(1);
}