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
    if (argc != 4)
    {
        printf("Usage: %s <server_ip> <server_port> <seed>\n", argv[0]);
        return -1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int seed = atoi(argv[3]);

    srand(seed);

    int sock, ret;
    struct sockaddr_in serv_addr;
    char *message = "hello";
    char buffer[BUF_SIZE] = {0};
    struct timeval start, end;
    double elapsed_time;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    printf("connecting\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed: %s\n", strerror(errno));
        return -1;
    }
    printf("connected\n");

    // Measure the start time
    gettimeofday(&start, NULL);

    // Random sleep to simulate real-world delay
    sleep(rand() % 5);

    ret = send(sock, message, strlen(message), 0);
    if (ret == -1)
    {
        printf("Failed to send message\n");
        return -1;
    }
    printf("Sent: %s\n", message);

    ret = recv(sock, buffer, BUF_SIZE, 0);
    if (ret == -1)
    {
        printf("Failed to receive message\n");
        return -1;
    }
    printf("Received: %s\n", buffer);

    // Measure the end time
    gettimeofday(&end, NULL);
    
    elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms

    printf("Response time: %.2f ms\n", elapsed_time);

    close(sock);

    return 0;
}
