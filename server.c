#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define DEFAULT_PORT 30000
#define MAX_RECORDS 512 // picked this number randomly
#define STOCK_STRING_LENGTH 10
#define DATE_STRING_LENGTH 11
#define MAX_MSG_LENGTH 255

typedef struct
{
    char date[DATE_STRING_LENGTH]; // YYYY-MM-DD format
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

double calculate_max_profit(const char *stock_name, const char *start_date, const char *end_date)
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
    int start_date_index = -1;
    int end_date_index = -1;
    for (int i = 0; i < records; i++)
    {
        if (strcmp(data[i].date, start_date) == 0)
        {
            start_date_index = i;
        }
        if (strcmp(data[i].date, end_date) == 0)
        {
            end_date_index = i;
        }
    }
    if (start_date_index == -1 || end_date_index == -1)
    {
        return -1; // Date not found
    }
    if (start_date_index >= end_date_index)
    {
        return -1; // Selling date is not later than buying date
    }
    double max_profit = 0;
    for (int buy_date_index = start_date_index; buy_date_index < end_date_index; buy_date_index++)
    {
        for (int sell_date_index = buy_date_index + 1; sell_date_index <= end_date_index; sell_date_index++)
        {
            double profit = data[sell_date_index].close_price - data[buy_date_index].close_price;
            if (profit > max_profit)
            {
                max_profit = profit;
            }
        }
    }
    return max_profit;
}

void read_stock_data(const char *filename, StockData *data, int *num_records)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return;
    }

    char line[MAX_MSG_LENGTH - 1];
    fgets(line, sizeof(line), file); // Read the header line and ignore it

    while (fgets(line, sizeof(line), file))
    {
        char *token;
        token = strtok(line, ","); // date
        strcpy(data[*num_records].date, token);

        for (int i = 0; i < 3; i++)
        { // skip next 3 columns
            token = strtok(NULL, ",");
        }
        data[*num_records].close_price = atof(strtok(NULL, ",")); // closing price

        (*num_records)++;
    }

    fclose(file);
}

int date_format_is_valid(char *date)
{
    if (strlen(date) != DATE_STRING_LENGTH - 1)
    {
        return -1;
    }
    else if (date[4] != '-' || date[7] != '-')
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    int port_number = DEFAULT_PORT;
    if (argc >= 3)
    {
        port_number = atoi(argv[3]);
    }

    read_stock_data("MSFT.csv", msft_data, &msft_records);
    read_stock_data("TSLA.csv", tsla_data, &tsla_records);

    printf("server started\n");
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

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
    address.sin_port = htons(port_number);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) // forcefully attaching socket to the port

    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // printf("Server is ready to accept a connection...\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    // printf("Connection accepted.\n");

    int done = 0;
    while (!done)
    {
        char receive_msg[MAX_MSG_LENGTH] = {0};
        int valread = read(new_socket, receive_msg, MAX_MSG_LENGTH); // read data from the client
        receive_msg[valread] = '\0';                                 // null-terminate the received string

        char receive_cmd[MAX_MSG_LENGTH - 1];
        strcpy(receive_cmd, &receive_msg[1]); // Extracting the command from the message

        printf("%s\n", receive_cmd);

        char response_string[MAX_MSG_LENGTH - 1];

        if (strcmp(receive_cmd, "List") == 0)
        {
            strcpy(response_string, "TSLA | MSFT"); // IDK if hardcoding this okay....
        }
        else if (strncmp(receive_cmd, "Prices", 6) == 0)
        {
            char stock_name[STOCK_STRING_LENGTH];
            char date[DATE_STRING_LENGTH];
            sscanf(receive_cmd, "Prices %s %s", stock_name, date); // Extract stock name and date

            if (date_format_is_valid(date) == 0)
            {
                double price = get_stock_price(stock_name, date);
                if (price >= 0)
                {
                    sprintf(response_string, "%.2f", price);
                }
                else
                {
                    sprintf(response_string, "Unknown");
                }
            }
            else
            {
                sprintf(response_string, "Invalid syntax");
            }
        }
        else if (strncmp(receive_cmd, "MaxProfit", 9) == 0)
        {
            char stock_name[STOCK_STRING_LENGTH];
            char start_date[DATE_STRING_LENGTH];
            char end_date[DATE_STRING_LENGTH];
            sscanf(receive_cmd, "MaxProfit %s %s %s", stock_name, start_date, end_date);
            if (date_format_is_valid(start_date) == 0 && date_format_is_valid(end_date) == 0)
            {
                double max_profit = calculate_max_profit(stock_name, start_date, end_date);
                if (max_profit >= 0)
                {
                    sprintf(response_string, "%.2f", max_profit);
                }
                else
                {
                    sprintf(response_string, "Unknown");
                }
            }
            else
            {
                sprintf(response_string, "Invalid syntax");
            }
        }
        else if (strcmp(receive_cmd, "quit") == 0)
        {
            strcpy(response_string, "Server: quitting...");
            done = 1;
        }
        else
        {
            strcpy(response_string, "Invalid syntax");
        }

        // Inserting the string_length as the first byte of the responding msg
        char response_msg[MAX_MSG_LENGTH];
        char string_length = strlen(response_string) + 1;
        response_msg[0] = string_length;
        strcpy(&response_msg[1], response_string);

        send(new_socket, response_msg, MAX_MSG_LENGTH, 0);
    }
    // printf("Server killed.\n");
    return 0;
}

/*
gcc -o server server.c
gcc -o client client.c


./server MSFT.csv TSLA.csv 30000
./client localhost 30000
*/