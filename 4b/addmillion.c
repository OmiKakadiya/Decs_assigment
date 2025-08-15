#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <assert.h>

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

long long int account_balance = 0;
pthread_mutex_t lock;

void *increment(void *arg)
{
    int to_incr=*(int *)arg;
    long long int local_balance=0;
    for (int j = 0; j < to_incr; j++)
    {
        
        for (int i = 0; i < 1000000; i++)
        {
            local_balance++;
        }

    }
    pthread_mutex_lock(&lock);
    account_balance+=local_balance;
    pthread_mutex_unlock(&lock);

}

int main(int argc, char *argv[])
{
    double start =GetTime();
    if (pthread_mutex_init(&lock,NULL)!=0)
    {
        /* code */
        printf("Error in mutex init");
        exit(EXIT_FAILURE);
    }


    int threadNum = atoi(argv[1]);
    pthread_t th[threadNum];
    int i;
    int k=2048/threadNum;   
     for (i = 0; i < threadNum; i++)
    {
        if (pthread_create(th + i, NULL, &increment, &k) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
        printf("Transaction %d has started\n", i);
        
    }
    for (i = 0; i < threadNum; i++)
    {
        
        if (pthread_join(th[i], NULL) != 0)
        {
            return 2;
        }
        printf("Transaction %d has finished\n", i);
    }
    printf("Account Balance is : %lld\n", account_balance);

    double end=GetTime();
    printf("Time spent: %f ms\n",(end-start)*1000);
    return 0;
}