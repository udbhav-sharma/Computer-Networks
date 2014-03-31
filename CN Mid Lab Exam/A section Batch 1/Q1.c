#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define portno_tcp 8019
#define portno_udp 9000

int sfd_tcp[2],sfd_udp[2];

struct sockaddr_in serv_addr_udp,serv_addr_tcp,cli_addr_tcp,cli_addr_udp[2];

int cli_len_tcp=sizeof(cli_addr_tcp);
int cli_len_udp[2];

int portno_udp_client[2];

void closesfd(int sig)
{
    int i;
    for(i=0;i<2;i++)
    {
        close(sfd_udp[i]);
        close(sfd_tcp[i]);
    }
    write(1,"Byee\n",5);
    exit(1);
}

main()
{
	(void )signal(SIGINT,closesfd);

    int i;
    
    portno_udp_client[0]=10000;
    portno_udp_client[1]=11000;
    
    for(i=0;i<2;i++)
    {
        sfd_tcp[i]=socket(AF_INET,SOCK_STREAM,0);
	    sfd_udp[i]=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

        if(sfd_tcp[i]<0)
	    {
		  perror("Error in creating socket stream\n");	
	    }

        if(sfd_udp[i]<0)
        {
            perror("Error in creating socket udp\n");
        }

        serv_addr_tcp.sin_family = AF_INET;
        serv_addr_tcp.sin_addr.s_addr = INADDR_ANY;
        serv_addr_tcp.sin_port = htons(portno_tcp+i);

        serv_addr_udp.sin_family = AF_INET;
        serv_addr_udp.sin_addr.s_addr = INADDR_ANY;
        serv_addr_udp.sin_port = htons(portno_udp+i);

        if(bind(sfd_tcp[i],(struct sockaddr *) &serv_addr_tcp, sizeof(serv_addr_tcp)) < 0)
        {
            perror("ERROR on binding in sfd_tcp");
            exit(0);
        }

        if(bind(sfd_udp[i],(struct sockaddr *) &serv_addr_udp, sizeof(serv_addr_udp)) < 0)
        {
            perror("ERROR on binding in sfd_udp");
            exit(0);
        }

        listen(sfd_tcp[i],5);
    }

    for(i=0;i<2;i++)
    {
        cli_addr_udp[i].sin_family = AF_INET;
        cli_addr_udp[i].sin_addr.s_addr = inet_addr("127.0.0.1");
        cli_addr_udp[i].sin_port = htons(portno_udp_client[i]);
        cli_len_udp[i]=sizeof(cli_addr_udp);
    }

    fd_set r_fds,r_fds2;
    FD_ZERO(&r_fds);
    FD_ZERO(&r_fds2);
    int max_fd=0,res;
    
    for(i=0;i<2;i++)
    {
        FD_SET(sfd_tcp[i],&r_fds);
        FD_SET(sfd_udp[i],&r_fds);

        FD_SET(sfd_tcp[i],&r_fds2);
        FD_SET(sfd_udp[i],&r_fds2);
        
        if(sfd_tcp[i]>max_fd)
            max_fd=sfd_tcp[i];
        
        if(sfd_udp[i]>max_fd)
            max_fd=sfd_udp[i];
    }

    struct timeval tv;
    tv.tv_usec=0;
    tv.tv_sec=5;
    
    while(1)
    {
        r_fds=r_fds2;

        res = select(max_fd+1, &r_fds, NULL, NULL,&tv);
        if(res>0)
        {
            for(i=0;i<2;i++)
            {
                if(FD_ISSET(sfd_tcp[i],&r_fds))
                {
                    int nsfd=accept(sfd_tcp[i],(struct sockaddr *)&cli_addr_tcp,&cli_len_tcp);
                    write(1,"Handling TCP service\n",strlen("Handling TCP service\n"));

                    char *service[1];
                    service[0]=(char*)malloc(100 * sizeof(char));
                    if(i==0)
                    	strcpy(service[0],"./uppercase");
                    else
                    	strcpy(service[0],"./lowercase");

                    int pid=fork();
                    if(pid>0)
                        close(nsfd);
                    else if(pid==0)
                    {
                    	close(sfd_tcp[i]);
                    	dup2(nsfd,0);
                    	dup2(nsfd,1);
                    	if(strcmp(service[0],"./uppercase")==0)
                    		execvp(service[0],service);
                    	else
                    		execvp(service[0],service);
                    }
                    
                }
                else if(FD_ISSET(sfd_udp[i],&r_fds))
                {
                    char buf[255];
                    write(1,"Handling UDP service\n",strlen("Handling UDP service\n"));
                    recvfrom(sfd_udp[i],buf,255,0,(struct sockaddr *) &cli_addr_udp[i],&cli_len_udp[i]);
                    write(1,buf,strlen(buf));
                    write(1,"\n",1);
                    sendto(sfd_udp[i],"Message received successfully through udp socket",
                        strlen("Message received successfully through udp socket"),0,(struct sockaddr *) &cli_addr_udp[i],cli_len_udp[i]);
                }
            }
        }
    }

    //Closing all sfds
    for(i=0;i<2;i++)
    {
        close(sfd_udp[i]);
        close(sfd_tcp[i]);
    }
    exit(1);
}