#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<errno.h>
#include<pthread.h>

#define LISTEN_BACKLOG 1024
#define BUF_SIZE 1024
#define MAXI 50

typedef struct {
    int fd;
}Task;


typedef struct {
    Task* tasks;
    int front;                  
    int rear;                   
    int size;
    int capacity;               
    pthread_mutex_t mutex;      // Mutex to protect queue access
    pthread_cond_t empty;        // Condition variable to track empty slots
    pthread_cond_t full;        // Condition variable to track full slots
    bool stop;                  // Flag to indicate no more tasks will be enqueued
} CircularQueue;

CircularQueue* createQueue(int capacity){
    CircularQueue* q = (CircularQueue*)malloc(sizeof(CircularQueue));
    q->tasks = (Task*)malloc(sizeof(Task)*capacity);
    q->front=0;
    q->rear=-1;
    q->size=0;
    q->capacity=capacity;
    if (pthread_mutex_init(&q->mutex,NULL)!=0)
    {
        printf("Mutex init error");
        free(q->tasks);
        free(q);
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&q->empty,NULL)!=0)
    {
        printf("empty init error");
        free(q->tasks);
        free(q);
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&q->full,NULL)!=0)
    {
        printf("Full init error");
        free(q->tasks);
        free(q);
        exit(EXIT_FAILURE);
    }  

    return q;  
    
}
void enqueue(CircularQueue* q, Task task) {
    pthread_mutex_lock(&q->mutex);

    // Wait if the queue is full
    while (q->size == q->capacity) {
        pthread_cond_wait(&q->empty, &q->mutex);
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->tasks[q->rear] = task;
    q->size++;

    // Signal that the queue is not empty
    pthread_cond_signal(&q->full);
    pthread_mutex_unlock(&q->mutex);
}

bool dequeueTask(CircularQueue* q, Task* task) {
    pthread_mutex_lock(&q->mutex);

    // Wait if the queue is empty
    while (q->size == 0) {
        pthread_cond_wait(&q->full, &q->mutex);
    }

    // Retrieve the task
    *task = q->tasks[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    // Signal that the queue is not full
    pthread_cond_signal(&q->empty);
    pthread_mutex_unlock(&q->mutex);
    return true;
}

void *fun(void* arg){
    CircularQueue *q = (CircularQueue*)arg;
    Task task;
    // int client_fd = *(int *)arg;
    char *message = "world";
    char buff[BUF_SIZE];
    int k;
 
    while (dequeueTask(q,&task))
    {
        if((k=(recv(task.fd,buff,BUF_SIZE-1,0)))>0)
        {
                buff[k]='\0';
                printf("Received buffer : %s\n",buff);
                if(strcmp(buff,"hello")==0){
                    send(task.fd,message,strlen(message),0);
                    memset(buff,0,sizeof(buff));
                }
        }
        else{
            printf("No data present ");
            pthread_exit(NULL);
        }
    }
    
    return NULL;
}
int main(int argc,char *argv[]){
    if(argc!=3){
        printf("give arguments port and thread_pool_size");
    }

    int port=atoi(argv[1]);
    int pool_size=atoi(argv[2]);
    
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


    CircularQueue* q = createQueue(MAXI);
    pthread_t p[pool_size];
    for(int i = 0; i < pool_size; i++)
    {
        if (pthread_create(p+i,NULL,fun,(void*)q)!=0)
        {
            printf("Thread Creation Error");
        }
    }
    
    while (1)
    {
        if ((client_fd=accept(sock,(struct sockaddr*)&client_addr,&addrlen) ) >=0)
        {  
           Task t;
           t.fd=client_fd;
           enqueue(q,t);
            
        }
        else{
             printf("Accept Error: %s\n", strerror(errno));
        }
       
    }

    return 0;
}

