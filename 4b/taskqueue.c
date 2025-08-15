#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<pthread.h>


typedef struct {
    char p;
    int time;
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
    bool stop;                  // To indicate no more tasks will be enqueued
} CircularQueue;



// Global variables
long sum = 0;
long odd = 0;
long even = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

pthread_mutex_t gblv ;

void processtask(long number);

void processtask(long number)
{
    // simulate burst time
    sleep(number);

    // update global variables
    
    pthread_mutex_lock(&gblv);
    sum += number;
    if (number % 2 == 1)
    {
        odd++;
    }
    else
    {
        even++;
    }
    if (number < min)
    {
        min = number;
    }
    if (number > max)
    {
        max = number;
    }
    pthread_mutex_unlock(&gblv);

}

CircularQueue* createQueue(int capacity) {
    CircularQueue* q = (CircularQueue*)malloc(sizeof(CircularQueue));
    if (!q) {
        perror("Failed to allocate memory for queue");
        exit(EXIT_FAILURE);
    }
    q->tasks = (Task*)malloc(sizeof(Task) * capacity);
    if (!q->tasks) {
        perror("Failed to allocate memory for queue tasks");
        free(q);
        exit(EXIT_FAILURE);
    }
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    q->capacity = capacity;
    q->stop = false;



    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        perror("Mutex init failed");
        free(q->tasks);
        free(q);
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_init(&q->empty, NULL) != 0) {
        perror("Condition variable init failed");
        pthread_mutex_destroy(&q->mutex);
        free(q->tasks);
        free(q);
        exit(EXIT_FAILURE); 
    }

    if (pthread_cond_init(&q->full, NULL) != 0) {
        perror("Condition variable init failed");
        pthread_mutex_destroy(&q->mutex);
        free(q->tasks);
        free(q);
        exit(EXIT_FAILURE);
    }

    return q;
}


void enqueue(CircularQueue* q, Task task) {
    pthread_mutex_lock(&q->mutex);

    // Wait if the queue is full
    while (q->size == q->capacity && !q->stop) {
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
    while (q->size == 0 && !q->stop) {
        pthread_cond_wait(&q->full, &q->mutex);
    }

    if (q->size == 0 && q->stop) {
        pthread_mutex_unlock(&q->mutex);
        return false; // No more tasks will be enqueued
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


void* workerFunction(void* arg) {
    CircularQueue *q = (CircularQueue*)arg;
    Task task;

    while (dequeueTask(q, &task)) {
        if (task.p == 'p') { // Processing task
            processtask(task.time);
             printf("Task completed\n");

        }
        else if (task.p == 'w') { // Waiting period
            sleep(task.time);
            printf("Wait Over\n");
        
        }
        else {
            printf("ERROR: Type Unrecognizable: '%c'\n", task.p);
            exit(EXIT_FAILURE);
        }
    }

    printf("Thread %lu exiting\n", pthread_self());
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: sum <infile>\n");
        exit(EXIT_FAILURE);
    }
    char *fn = argv[1];
    int no_threads=atoi(argv[2]);
    pthread_t p[no_threads];
    // Read from file

    if (pthread_mutex_init(&gblv,NULL)!=0)
    {
        printf("Error in mutex init");
    }
    
    FILE *fin = fopen(fn, "r");
    long t;
    fscanf(fin, "%ld\n", &t);
    printf("The number of tasks are : %ld \n", t);
    char type;
    long num;
        CircularQueue* q = createQueue(t);
        for (int i = 0; i < no_threads; i++)
        {
            pthread_create(p+i,NULL,workerFunction,(void *)q);
        }
        
        while (fscanf(fin, "%c %ld\n", &type, &num) == 2)
        {
            Task t ;
            t.p=type;
            t.time=num;
            enqueue(q,t);
        }

          pthread_mutex_lock(&q->mutex);
            q->stop = true;  // Set stop flag
            pthread_cond_broadcast(&q->full);  // Wake up all waiting threads
            pthread_mutex_unlock(&q->mutex);

    // while (fscanf(fin, "%c %ld\n", &type, &num) == 2)
    // {
    //     if (type == 'p')
    //     { // processing task
    //         processtask(num);
    //         printf("Task completed\n");
    //     }
    //     else if (type == 'w')
    //     { // waiting period
    //         sleep(num);
    //         printf("Wait Over\n");
    //     }
    //     else
    //     {
    //         printf("ERROR: Type Unrecognizable: '%c'\n", type);
    //         exit(EXIT_FAILURE);
    //     }
    // }
    // fclose(fin);

    for (int i = 0; i < no_threads; i++)
    {
        if (pthread_join(p[i],NULL)!=0)
        {
            printf("Thread_Join Error");
        }
        
    }
    
    // Print global variables
    printf("%ld %ld %ld %ld %ld\n", sum, odd, even, min, max);

    return (EXIT_SUCCESS);
}
