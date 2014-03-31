#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>



int main(int argc, char* argv[])
{
    char *domain = argv[1];
    char *path = strchr(domain, '/');
    *path++ = '\0';

    printf("host: %s; path: %s\n", domain, path);

    int sock, bytes_recieved;  
    char send_data[1024],recv_data[9999];
    struct sockaddr_in server_addr;
    struct hostent *he;

    he = gethostbyname(domain);
    if (he == NULL){
       herror("gethostbyname");
       exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0))== -1){
       perror("Socket");
       exit(1);
    }
    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(server_addr.sin_zero),8); 
    if (connect(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){
       perror("Connect");
       exit(1); 
    }

   snprintf(send_data, sizeof(send_data), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);
   printf("%s\n", send_data);
   send(sock, send_data, strlen(send_data), 0); 
   printf("Data sent.\n");  
   bytes_recieved = recv(sock, recv_data, 9999, 0);
   recv_data[bytes_recieved] = '\0';
   close(sock);
   printf("Data revceived.\n");
   printf("%s\n", recv_data);


return 0;
} 