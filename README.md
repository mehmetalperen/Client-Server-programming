# Stock Market Information System
## Overview
This project implements a networked client/server application for querying stock market information for TESLA (TSLA) and Microsoft (MSFT) stocks. Users can interact with the client program to request and display stock prices on specified dates and calculate maximum possible profits within a given date range.
## Getting Started
### Dependencies
* Ensure you have gcc installed on your system to compile the source code. This project has been tested on openlab.ics.uci.edu.
* The application is designed to run on Linux-based systems.

### Installation
1) Clone this repository or download the source files.
2) Navigate to the project directory.
3) Compile the client and server programs using the following commands:
```
gcc -o client client.c
gcc -o server server.c
```
### Running the Application
#### Server
Start the server program with the following command:
```
./server MSFT.csv TSLA.csv <PortNumber>
```
* <PortNumber> should be replaced with an available port number between 30000 and 60000.
1) The server will read the stock data from the provided CSV files and start listening for client requests on the specified port.

#### Client
1) Start the client program with the following command:
```
./client <ServerAddress> <PortNumber>
```
* <ServerAddress> is the domain name or IP address of the server. Use localhost if running on the same machine.
* <PortNumber> should match the port number the server is listening on.
2) After starting, the client will display a prompt (> ) waiting for input commands.
### Usage
#### Client Commands
* List: Displays the available stocks (TSLA and MSFT).
* Prices <StockSymbol> <Date>: Displays the closing price of the specified stock on the given date.
* MaxProfit <StockSymbol> <StartDate> <EndDate>: Calculates the maximum possible profit for the specified stock within the given date range. quit: Exits the client application.
#### Examples
```
> List
TSLA | MSFT
> Prices MSFT 2021-11-08
269.36
> MaxProfit TSLA 2021-11-04 2021-11-17
6.34
> quit
```
### Known issues
* The application does not handle network errors gracefully.
* The system assumes stock data is complete and does not account for missing dates.
