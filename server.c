#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 30000

int main(int argc, char *argv[])
{
    printf("Running server...\n");
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0}; // Buffer for storing received data

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 30000
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 30000
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server is ready to accept a connection...\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted.\n");

    while (1)
    {
        valread = read(new_socket, buffer, 1024); // Read data from the client
        buffer[valread] = '\0';                   // Null-terminate the received string

        char dummyData[50] = "This is dummy data.";

        // Handle 'list' command
        if (strcmp(buffer, "list") == 0)
        {
            // Logic to get list of stocks (e.g., "TSLA | MSFT")
            // Send response back to client
            strcpy(dummyData, "To be implemented: list");
            send(new_socket, dummyData, strlen(dummyData), 0); // Send dummy data
        }
        // Handle 'prices' command
        else if (strncmp(buffer, "prices", 6) == 0)
        {
            // Parse stock name and date from buffer
            // Logic to get stock price for the given date
            // Send response back to client
            strcpy(dummyData, "To be implemented: prices");
            send(new_socket, dummyData, strlen(dummyData), 0); // Send dummy data
        }
        // Handle 'maxProfit' command
        else if (strncmp(buffer, "maxProfit", 9) == 0)
        {
            // Parse stock name, start date, and end date from buffer
            // Logic to calculate max profit
            // Send response back to client
            strcpy(dummyData, "To be implemented: maxProfit");
            send(new_socket, dummyData, strlen(dummyData), 0); // Send dummy data
        }
        else if (strcmp(buffer, "quit") == 0)
        {
            strcpy(dummyData, "Server: quiting...");
            send(new_socket, dummyData, strlen(dummyData), 0); // Send dummy data
            break;
        }

        // Send response back to client
        // ...
    }
    printf("Server killed.");
    return 0;
}

/*
gcc -o server server.c
gcc -o client client.c


./server MSFT.csv TSLA.csv 30000
./client localhost 30000
*/