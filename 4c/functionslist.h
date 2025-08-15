#ifndef FUNCTIONSLIST_H
#define FUNCTIONSLIST_H
#include "functions.h"

// Function declarations for the operations
char* root(void *arg);
char* square(void *arg);
char* cube(void *arg);
char* hello(void *arg);
char* prime(void *arg);
char* pingpong(void *arg);
char* fibo(void* arg);

// Define a function pointer type for the operations
typedef char* (*operation_func)(void *);

// Create an array of function pointers
static operation_func functionList[] = {
    root,      // ROOT
    square,    // SQUARE
    cube,      // CUBE
    hello,     // HELLO
    prime,     // PRIME
    pingpong,   // PINGPONG
    fibo
};

#endif
