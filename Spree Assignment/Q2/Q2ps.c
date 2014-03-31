#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define maxnumberofconnections 20

int sfd,portno;
struct sockaddr_in serv_addr, cli_addr[maxnumberofconnections];
int clilen[maxnumberofconnections];
int nsfd[maxnumberofconnections];
int nsfd_size=0;

int max_fd=0;
fd_set fdarray, fdarray2;

struct Vertex_Details
{
	//Main details
	int sfd;
	char name[255];
	//For Strong Component Purpose
	int start;
	int finish;
	int visited;
	int Strong_Component;
};

struct Vertex_Details vertices[maxnumberofconnections];
int vertices_size=0;
int **edge;
int num=0;

void initialize_edge()
{
    int i,j;

    edge=(int**)malloc(sizeof(int*)*maxnumberofconnections);
    for(i=0;i<maxnumberofconnections;i++)
        edge[i]=(int*)malloc(sizeof(int)*maxnumberofconnections);

    for(i=0;i<maxnumberofconnections;i++)
        for(j=0;j<maxnumberofconnections;j++)
            edge[i][j]=0;
}

int get_index_sfd(int sfd)
{
	int i=0;
	for(i=0;i<vertices_size;i++)
		if(vertices[i].sfd==sfd)
			return i;
	return -1;
}

int get_index_name(char buffer[255])
{
	int i=0;
	for(i=0;i<vertices_size;i++)
		if(strcmp(vertices[i].name,buffer)==0)
			return i;
	return -1;
}

void add_vertex(char buffer[255],int sfd)
{
	vertices[vertices_size].sfd=sfd;
	strcpy(vertices[vertices_size].name,buffer);
	vertices_size++;
}

void add_edge(int sfd,char buffer[255])
{
	int i=get_index_sfd(sfd);
	int j=get_index_name(buffer);

	edge[i][j]=1;
}

void DFS(int i)
{
	vertices[i].visited=1;
	num++;
	vertices[i].start=num;
	int j;
	for(j=0;j<maxnumberofconnections;j++)
		if(edge[i][j] && !vertices[j].visited)
			DFS(j);
	num++;
	vertices[i].finish=num;
}

void DFS_T(int i)
{
	vertices[i].visited=1;
	int j;
	for(j=0;j<maxnumberofconnections;j++)
		if(edge[j][i])
		{
			vertices[j].Strong_Component=vertices[i].Strong_Component;
			if(!vertices[j].visited)
				DFS_T(j);
		}
}

int comp (const void * elem1, const void * elem2) 
{
    struct Vertex_Details f = *((struct Vertex_Details*)elem1);
    struct Vertex_Details s = *((struct Vertex_Details*)elem2);
    if (f.finish >=s.finish) return  1;
    if (f.finish < s.finish) return -1;
    return 0;
}

void print_graph(int** Graph)
{
	int i,j;
	printf("\t");
	for(i=0;i<vertices_size;i++)
	{
		printf("%s\t",vertices[i].name);
	}
	printf("\n");
	for(i=0;i<vertices_size;i++)
	{
		printf("%s\t",vertices[i].name);
		for(j=0;j<vertices_size;j++)
		{
			printf("%d\t",Graph[i][j]);
		}
		printf("\n");
	}
}

void print_Vertices()
{
	int i;
	for(i=0;i<vertices_size;i++)
	{
		printf("%s %d %d %d %d\n",vertices[i].name,vertices[i].sfd,vertices[i].start,vertices[i].finish,vertices[i].Strong_Component);
	}
}

void Find_Strong_Components()
{
	num=0;
	int i;
	for(i=0;i<vertices_size;i++)
		vertices[i].visited=0;

	for(i=0;i<vertices_size;i++)
		if(!vertices[i].visited)
			DFS(i);

	struct Vertex_Details temp_vertices[maxnumberofconnections];
	for(i=0;i<vertices_size;i++)
		temp_vertices[i]=vertices[i];

	qsort(temp_vertices,(sizeof(struct Vertex_Details)*vertices_size)/sizeof(*temp_vertices),sizeof(*temp_vertices),comp);

	/*return;*/
	//Final Step
	//print_Vertices();

	for(i=0;i<vertices_size;i++)
		vertices[i].visited=0;

	for(i=vertices_size-1;i>=0;i--)
	{
		int j=get_index_sfd(temp_vertices[i].sfd);
		if(!vertices[j].visited)
		{
			vertices[j].Strong_Component=vertices[j].sfd;
			DFS_T(j);
		}
	}

	print_graph(edge);
	print_Vertices();
}

void *accept_connections()
{
    initialize_edge();

    char buffer[255];
    nsfd_size=0;

    FD_ZERO(&fdarray);
    FD_ZERO(&fdarray2);
    
    while(1)
    {
    	clilen[nsfd_size]=sizeof(cli_addr[nsfd_size]);
    	nsfd[nsfd_size]=accept(sfd,(struct sockaddr *)&cli_addr[nsfd_size],
    		&clilen[nsfd_size]);

    	if(nsfd[nsfd_size]<0) 
    	{
    		perror("ERROR on accept");
    		exit(0);
    	}

    	//Name of client
    	recv(nsfd[nsfd_size],buffer,255,0);
    	write(1,"Name of client:\t",strlen("Name of client:\t"));
    	write(1,buffer,strlen(buffer));
    	write(1,"\n",1);
    	add_vertex(buffer,nsfd[nsfd_size]);

    	FD_SET(nsfd[nsfd_size],&fdarray);
    	FD_SET(nsfd[nsfd_size],&fdarray2);

    	if(max_fd<nsfd[nsfd_size])
    		max_fd=nsfd[nsfd_size];

    	write(1,"Connection Created\n",strlen("Connection Created\n"));
    	nsfd_size++;
    }
}

void send_msg(char buffer[255],int sfd)
{
	int i;
	int new_sfd=vertices[get_index_sfd(sfd)].Strong_Component;

	for(i=0;i<vertices_size;i++)
		if(vertices[i].Strong_Component==new_sfd && vertices[i].sfd!=sfd)
		{
			write(1,"send_msg called\n",strlen("send_msg called\n"));
			send(vertices[i].sfd,buffer,strlen(buffer),0);
		}
}

void *read_from_client()
{
    char buffer[255];

    struct timeval tv;
    tv.tv_usec=0;
    tv.tv_sec=2;

    while(1)
    {
        fdarray=fdarray2;

        int ret=select(max_fd+1, &fdarray, NULL, NULL,&tv);
        int i;
        if(ret>0)
        {
            for(i=0;i<nsfd_size;i++)
            {
                if(FD_ISSET(nsfd[i],&fdarray))
                {
                	recv(nsfd[i],buffer,255,0);
                	write(1,buffer,strlen(buffer));
                	write(1,"\n",1);
                	if(strcmp(buffer,"#List")!=0)
                		send_msg(buffer,nsfd[i]);
                	else
                	{
						//Adjacency List
    					write(1,"Adjacenecy List\n",strlen("Adjacenecy List\n"));
                		recv(nsfd[i],buffer,255,0);
                		write(1,buffer,strlen(buffer));
                		write(1,"\n",1);
                		
                		while(strcmp(buffer,"-1")!=0)
                		{
                			add_edge(nsfd[i],buffer);
                			recv(nsfd[i],buffer,255,0);
                			write(1,buffer,strlen(buffer));
                			write(1,"\n",1);
                		}

                		Find_Strong_Components();
                	}
                }
            }
        }
    }
}

void close_all_sfds()
{
    int i;
    for(i=0;i<nsfd_size;i++)
        close(nsfd[i]);
    close(sfd);
    write(1,"Hv a Wonderful Day\n",strlen("Hv a Wonderfull Day\n"));
}

void closesfd(int sig)
{
    close_all_sfds();
    exit(1);
}

main()
{
    (void )signal(SIGINT,closesfd);

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno =9006;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,maxnumberofconnections);

    //Making threads to handle accept and read
    pthread_t connection_thread,read_thread;
    int error=pthread_create(&connection_thread,NULL,accept_connections,NULL);
    if(error<0)
    {
        perror("ERROR in connection_thread:");
        exit(0);
    }
    error=pthread_create(&read_thread,NULL,read_from_client,NULL);
    if(error<0)
    {
        perror("ERROR in read_thread:");
        exit(0);
    }
    pthread_join( connection_thread, NULL);
    pthread_join( read_thread, NULL);

    //Closing the open sockets
    close_all_sfds();

	exit(1);
}