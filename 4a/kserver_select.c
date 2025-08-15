#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/select.h>
#include<time.h>
#include<errno.h>

#define LISTEN_BACKLOG 5
#define BUF_SIZE 50


int main(int argc,char *argv[])
{
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
    server_addr.sin_port=htons(8089);
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

    fd_set master_set,working_set;
    FD_ZERO(&master_set);
    FD_ZERO(&working_set);
    int max_fd;
    FD_SET(sock,&master_set);
    max_fd=sock;

     printf("Server listening on 127.0.0.1:8089\n");
    char *message = "world";
    char buff[BUF_SIZE];
    int k;
    socklen_t addrlen = sizeof(client_addr);

    
    while (1)
    {

        working_set=master_set;
        int activity=select(max_fd+1,&working_set,NULL,NULL,0);
        // printf("active\n");
        for (int i = 0; i <=max_fd; i++)
        {
            if (FD_ISSET(i,&working_set))
            {
                if (i==sock)
                {
                    if ((client_fd=accept(sock,(struct sockaddr*)&client_addr,&addrlen) )  >0)
                    {
                        FD_SET(client_fd, &master_set);
                         if (client_fd > max_fd) {
                             max_fd = client_fd;
                        }
                        printf("client %d connected \n",client_fd); 
                    }
                }
                else
                {
                        memset(buff, 0, sizeof(buff));
                         if ((k = recv(i, buff, BUF_SIZE - 1, 0)) > 0) {
                            buff[k]='\0';
                            printf("Received buffer from client %s : %d\n",buff,i);
                            if(strcmp(buff,"hello")==0){
                                send(i,message,strlen(message),0);
                                 FD_CLR(i, &master_set); 
                                 close(i);
                             }
                           
                            
                        }
                        else if (k < 0) {
                            printf("Rev error : %s\n", strerror(errno));
                        }
                }
          }
        }
    }
    return 0;

}



