#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <stdlib.h>

#define NUM 3

int sfd[NUM];

int main(int argc , char *argv[])
{
    int client_sock , i, c , read_size;
    struct sockaddr_in server , client;
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    int port_num = 9080;
    
    for (i=0; i<NUM; i++)
    {
    	sfd[i] = socket(AF_INET , SOCK_STREAM , 0);
    	server.sin_port = htons( port_num+i );
    	bind(sfd[i],(struct sockaddr *)&server , sizeof(server));
    	listen(sfd[i], 3);
    }
    puts("Waiting for incoming connections...");
    
    
    fd_set r_fds;
    FD_ZERO(&r_fds);
    int max_fd=0, res;
    
    for (i=0; i<NUM; i++)
    {
    	FD_SET(sfd[i], &r_fds);
    	if (sfd[i] > max_fd)
    		max_fd = sfd[i];
    }
    
    res = select(max_fd+1, &r_fds, NULL, NULL, NULL);

    if (res > 0)
    {
    	for (i=0; i<NUM; i++)
    	{
    		if ( FD_ISSET(sfd[i], &r_fds) )
    		{	
    			client_sock = accept(sfd[i], (struct sockaddr *)&client, (socklen_t*)&c);
    			printf("Client received by sfd - %d\n", i+1);
    			break;
    		}
    	}
    }
    for(i=0; i<NUM; i++)
	close(sfd[i]);
}
