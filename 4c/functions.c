#include <stdio.h>
#include <string.h>
#include "functionslist.h"

char str[200]; 

char* root(void* arg) {
    char *ans = "Hello World";
    return ans;
}

char* square(void *arg) {
    int x =*((int *)arg);
    if(x==0){
        strcpy(str, "1");
        return str;
    }
    int number =x*x;
    sprintf(str, "%d", number);
    return  str;
}

char* cube(void *arg) {
        int x =*((int *)arg);
    if(x==0){
           strcpy(str, "1");
           return str;

    } 
    int number =x*x*x;
    sprintf(str, "%d", number);
    return  str;
}

char* hello(void * arg) {
    char *c =(char *)arg;
    strcpy(str,"Hello");
    if(c){
        // printf("%s",c);
        strcat(str," ");
        strcat(str,c);
    }
    return str;
}

char* prime(void * arg) {
                int x =*((int *)arg);
    int is_prime = 1;
    for (int i = 2; i*i<= x; i++) {
        if (x % i == 0) {
            is_prime = 0;
            break;
        }
    }
    strcpy(str,"true");
    if(is_prime){
        return str;
    }
    strcpy(str,"");
    strcpy(str,"false");
    return str;
}

char* pingpong(void *arg) {
        char *c =(char *)arg;
    if(c){
        // printf("%s",c);
        strcpy(str,c);
        return str;
    }
    // printf("ckkfkw");
    strcpy(str,"PingPong");
    return str;
}

char* fibo(void *arg){
    int x =*((int *)arg);
    if(x==0){
           strcpy(str, "1");
           return str;

    }
    if(x==1 || x==2){
           strcpy(str, "1");
           return str;

    }
    int first =1;
    int second=1;
    int ans;
    x-=2;
    int prev;
    while (x--)
    {
        prev=first;
        first+=second;
        second=prev;
    }
    int number =first;
    sprintf(str, "%d", number);
    return  str;
    
}

