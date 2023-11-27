#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 30000
#define MAX_MSG_LENGTH 255

int main(int argc, char *argv[])
{
    printf("Client...\n");
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char send_cmd[MAX_MSG_LENGTH - 1] = {0};
    char receive_msg[MAX_MSG_LENGTH] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1)
    {
        printf("> ");
        fgets(send_cmd, MAX_MSG_LENGTH - 1, stdin);
        send_cmd[strcspn(send_cmd, "\n")] = 0;
        
        char send_msg[MAX_MSG_LENGTH]; // First byte contains the string length, the rest contains the string
        char string_length = strlen(send_cmd) + 1; // add one for the null-terminator
        send_msg[0] = string_length; // Put string length to the first byte of message
        strcpy(&send_msg[1], send_cmd); // Put the string to the rest of the message

        send(sock, send_msg, MAX_MSG_LENGTH, 0); // send message to server

        if (strcmp(send_cmd, "quit") == 0)
        {
            break;
        }

        // receive and display response from server
        valread = read(sock, receive_msg, MAX_MSG_LENGTH);
        receive_msg[valread] = '\0'; // Null-terminate the received string
        printf("%s\n", &receive_msg[1]); // Print everything after the first byte
    }
    printf("Client killed.\n");

    return 0;
}
