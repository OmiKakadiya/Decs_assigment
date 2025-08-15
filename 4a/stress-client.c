#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: %s <server_ip> <server_port> <seed> <num_requests> \n", argv[0]);
        return -1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int seed = atoi(argv[3]);
    int num_requests = atoi(argv[4]);

    srand(seed);

    int sock, ret;
    struct sockaddr_in serv_addr;
    char *message = "hello";
    char buffer[BUF_SIZE] = {0};
    struct timeval start, end;
    double elapsed_time, total_time = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    printf("Connecting to the server...\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed: %s\n", strerror(errno));
        return -1;
    }
    printf("Connected to the server.\n");

    // Measure the start time before sending multiple requests
            gettimeofday(&start, NULL);  // Measure start time for each request

    for (int i = 0; i < num_requests; i++) {

        ret = send(sock, message, strlen(message), 0);
        if (ret == -1)
        {
            printf("Failed to send message\n");
            return -1;
        }

        ret = recv(sock, buffer, BUF_SIZE, 0);
        if (ret == -1)
        {
            printf("Failed to receive message\n");
            return -1;
        }


       

        // Clear buffer for next request
        memset(buffer, 0, BUF_SIZE);
    }
            gettimeofday(&end, NULL);  // Measure end time for each request


    printf("Average response time: %.2f ms\n", total_time / num_requests);
     elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
        elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms
        total_time += elapsed_time;

        printf(": Response time: %.2f ms\n", elapsed_time);

    close(sock);
    
    // Removed free(message); since message is not dynamically allocated

    return 0;
}
