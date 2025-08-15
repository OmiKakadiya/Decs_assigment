#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>

#define LISTEN_BACKLOG 5000
#define BUF_SIZE 50
#define MAX_EVENTS 10

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set");
        return -1;
    }
    return 0;
}

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
    server_addr.sin_port=htons(8080);
    if(bind(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
        printf("Bind error\n");
        return -1;
    }

         printf("Server listening on 127.0.0.1:8080\n");
    //listen
    if (listen(sock,LISTEN_BACKLOG)<0)
    {
        printf("Listen error\n");
        return -1;
    }
   
    char *message = "world";
    char buff[BUF_SIZE];
    int k;

    //creating epoll instance
    int epoll_fd=epoll_create1(0);
    if (epoll_fd==-1)
    {
        printf("Epoll creation error\n");
        return -1;
    }
     
    struct epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=sock;
    if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sock,&ev)==-1)
    {
        printf("epoll ctl error");
        return -1;
    }

    struct epoll_event events[MAX_EVENTS];
    
    while (1)
    {
        int nfds = epoll_wait(epoll_fd,events,MAX_EVENTS,-1);    
        if (nfds==-1)
        {
              perror("epoll_wait");
        }
        
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd==sock)
            {
                    socklen_t addrlen =sizeof(client_addr);
                    if ((client_fd=accept(sock,(struct sockaddr*)&client_addr,&addrlen))<0)
                    {
                            printf("Accept Error\n");
                    }
                    set_nonblocking(client_fd);
                    ev.events=EPOLLIN |EPOLLET;
                    ev.data.fd=client_fd;
                    if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&ev)<0)
                    {
                       perror("epoll_ctl: conn_sock\n");
                    }             
            }
            else if(events[i].events & EPOLLIN){
                client_fd=events[i].data.fd;
                if((k=(recv(client_fd,buff,BUF_SIZE-1,0)))>0)
                {
                    buff[k]='\0';
                    printf("Received buffer from client %d: %s\n",client_fd,buff);
                    if(strcmp(buff,"hello")==0){
                        send(client_fd,message,strlen(message),0);
                        memset(buff,0,sizeof(buff));
                        if(epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,&ev)<0){
                            perror("epoll_del: del error\n");
                        }
                        close(client_fd);
                    }

                }
                else{
                    printf('rec error %s\n',strerror(errno));
                }
            }

               
        }
        
    }

    close(epoll_fd);
    close(sock);
    return 0;
}
