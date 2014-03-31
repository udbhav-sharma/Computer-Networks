#include <stdio.h>
#include <string.h>    
#include <sys/socket.h>    
#include <arpa/inet.h> 
 
int main(int argc , char *argv[])
{
    int sock, port_num, u_inp;
    printf("Enter which server to connect (1-3) :: ");
    scanf("%d",&u_inp);
    
    struct sockaddr_in server;
     
    sock = socket(AF_INET , SOCK_STREAM , 0);
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    
    port_num = 9080 + (u_inp - 1);
    server.sin_port = htons( port_num );
 
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    	printf("Failed\n");
    else
    	printf("Connected\n");

    close(sock);
    return 0;
}
