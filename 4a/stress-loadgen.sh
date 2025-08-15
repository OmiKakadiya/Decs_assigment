#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <number_of_clients> <server_ip> <server_port> <num_requests_per_client>"
    exit 1
fi

NUM_CLIENTS=$1
SERVER_IP=$2
SERVER_PORT=$3
NUM_REQUESTS=$4

# Compile the client program
gcc stress-client.c -o stress-client

if [ $? -ne 0 ]; then
    echo "Compilation of stress-client.c failed"
    exit 1
fi

echo "Starting $NUM_CLIENTS clients to connect to $SERVER_IP:$SERVER_PORT with $NUM_REQUESTS requests each"

# Start clients in the background and store their PIDs
for ((i = 1; i <= $NUM_CLIENTS; i++)); do
    echo "Starting client #$i"
    ./stress-client $SERVER_IP $SERVER_PORT $i $NUM_REQUESTS &
done

echo "All clients have finished."
