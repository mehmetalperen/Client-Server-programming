#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 30000
#define MAX_RECORDS 512 // picked this number randomly

typedef struct
{
    char date[11]; // YYYY-MM-DD format
    double close_price;
} StockData;

StockData msft_data[MAX_RECORDS];
StockData tsla_data[MAX_RECORDS];
int msft_records = 0, tsla_records = 0;

double get_stock_price(const char *stock_name, const char *date)
{
    StockData *data;
    int records;

    if (strcmp(stock_name, "MSFT") == 0)
    {
        data = msft_data;
        records = msft_records;
    }
    else if (strcmp(stock_name, "TSLA") == 0)
    {
        data = tsla_data;
        records = tsla_records;
    }
    else
    {
        return -1; // Stock not found
    }

    for (int i = 0; i < records; i++)
    {
        if (strcmp(data[i].date, date) == 0)
        {
            return data[i].close_price;
        }
    }
    return -1; // Date not found
}

void read_stock_data(const char *filename, StockData *data, int *num_records)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return;
    }

    char line[1024];
    fgets(line, sizeof(line), file); // Read the header line and ignore it

    while (fgets(line, sizeof(line), file))
    {
        char *token;
        token = strtok(line, ","); // date
        strcpy(data[*num_records].date, token);

        for (int i = 0; i < 4; i++)
        { // skip next 4 columns
            token = strtok(NULL, ",");
        }
        data[*num_records].close_price = atof(strtok(NULL, ",")); // closing price

        (*num_records)++;
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    read_stock_data("MSFT.csv", msft_data, &msft_records);
    read_stock_data("TSLA.csv", tsla_data, &tsla_records);

    int msft_records = 0, tsla_records = 0;

    printf("Running server...\n");
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // creating socket file descriptor
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) // forcefully attaching socket to the port 30000

    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) // forcefully attaching socket to the port 30000

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
        valread = read(new_socket, buffer, 1024); // read data from the client
        buffer[valread] = '\0';                   // null-terminate the received string

        char response[100] = "This is dummy data.";

        if (strcmp(buffer, "list") == 0)
        {
            strcpy(response, "TSLA | MSFT"); // IDK if hardcoding this okay....
            send(new_socket, response, strlen(response), 0);
        }

        else if (strncmp(buffer, "prices", 6) == 0)
        {
            char stock_name[10];
            char date[11];
            sscanf(buffer, "prices %s %s", stock_name, date); // Extract stock name and date

            double price = get_stock_price(stock_name, date);

            if (price >= 0)
            {
                sprintf(response, "Price on %s for %s: %.2f", date, stock_name, price);
            }
            else
            {
                sprintf(response, "Data not found for %s on %s", stock_name, date);
            }

            send(new_socket, response, strlen(response), 0);
        }

        else if (strcmp(buffer, "maxProfit") == 0)
        {
            // Parse stock name, start date, and end date from buffer
            // Logic to calculate max profit
            // Send response back to client
            strcpy(response, "To be implemented: maxProfit");
            send(new_socket, response, strlen(response), 0);
        }
        else if (strcmp(buffer, "quit") == 0)
        {
            strcpy(response, "Server: quiting...");
            send(new_socket, response, strlen(response), 0);
            break;
        }
        else
        {
            strcpy(response, "Unknown request from client. Send a valid request...");
            send(new_socket, response, strlen(response), 0);
        }
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