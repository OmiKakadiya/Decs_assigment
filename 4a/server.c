#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<errno.h>


#define LISTEN_BACKLOG 1024 
#define BUF_SIZE 1024

int main(int argc,char *argv[]){
    int sock,client_fd;
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
    server_addr.sin_port=htons(8088);
    if(bind(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
        printf("Bind error\n : %s",strerror(errno));
        return -1;
    }

    //listen
    if (listen(sock,LISTEN_BACKLOG)<0)
    {
        printf("Listen error\n");
        return -1;
    }
   
    char *message = "world";
    char buff[BUF_SIZE];
    int k;
    socklen_t addrlen = sizeof(client_addr);
     printf("Server listening on 127.0.0.1:9099\n");

    while (1)
    {
        // sleep(100000);
        if ((client_fd=accept(sock,(struct sockaddr*)&client_addr,&addrlen) ) >0)
        {
             if((k=(recv(client_fd,buff,BUF_SIZE-1,0)))>0)
            {
                buff[k]='\0';
                printf("Received buffer : %s\n",buff);
                if(strcmp(buff,"hello")==0){
                    send(client_fd,message,strlen(message),0);
                    memset(buff,0,sizeof(buff));
                    close(client_fd);
                }
            }
            else{
                printf("Rec error :%s\n",strerror(errno));
            }
        }
        else{
            printf("Accept Error: %s\n", strerror(errno));

        }
       
    }

    return 0;
}


