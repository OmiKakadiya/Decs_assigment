#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<errno.h>
#include<pthread.h>

#define LISTEN_BACKLOG 1024
#define BUF_SIZE 1024

void *fun(void* arg){
    int client_fd = *(int *)arg;
     char *message = "world";
    char buff[BUF_SIZE];
    int k;
     if((k=(recv(client_fd,buff,BUF_SIZE-1,0)))>0)
    {
            buff[k]='\0';
            printf("Received buffer : %s\n",buff);
            if(strcmp(buff,"hello")==0){
                send(client_fd,message,strlen(message),0);
                memset(buff,0,sizeof(buff));
            }
    }
    else{
        printf("No data present ");
        pthread_exit(NULL);
    }
        close(client_fd);
        pthread_exit(NULL);
}
int main(int argc,char *argv[]){
    int sock,client_fd;
    
    // pthread_t p[no_threads];
    pthread_t thread;
    
    struct sockaddr_in  server_addr,client_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return -1;
    }

    //bind
    memset(&server_addr,0,sizeof(server_addr));

    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr.sin_port=htons(8080);
    if(bind(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
        printf("Bind error\n");
        return -1;
    }

    //listen
    if (listen(sock,LISTEN_BACKLOG)<0)
    {
        printf("Listen error\n");
        return -1;
    }
   
    socklen_t addrlen = sizeof(client_addr);
     printf("Server listening on 127.0.0.1:8080\n");


    while (1)
    {
        if ((client_fd=accept(sock,(struct sockaddr*)&client_addr,&addrlen) ) >=0)
        {  
            int *new_client_fd = malloc(sizeof(int));
            *new_client_fd = client_fd;

             if (pthread_create(&thread,NULL,fun,new_client_fd)!=0)
            {
                printf("Thread Creation error");
            }
            pthread_detach(thread);    
            
        }
        else{
             printf("Accept Error: %s\n", strerror(errno));
        }
       
    }

    return 0;
}

