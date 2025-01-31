#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstdio>
#include "card.hpp"

#define PORT 2000 // tcp port
#define PORT_UDP 2001
#define BUFFER_SIZE 1024

ssize_t sendData(int socket, const char *data, size_t length, const struct sockaddr_in *destAddr = nullptr)
{
    if (destAddr == nullptr)
    {
        return send(socket, data, length, 0); // TCP
    }
    else
    {
        return sendto(socket, data, length, 0, (const struct sockaddr *)destAddr, sizeof(*destAddr)); // UDP
    }
}

ssize_t readData(int socket, char *buffer, size_t bufferSize, struct sockaddr_in *srcAddr = nullptr)
{
    if (srcAddr == nullptr)
    {
        return read(socket, buffer, bufferSize); // TCP
    }
    else
    {
        socklen_t addrlen = sizeof(*srcAddr);
        return recvfrom(socket, buffer, bufferSize, 0, (struct sockaddr *)srcAddr, &addrlen); // UDP
    }
}

void splitString(const std::string &input, char delimiter, int &num1, int &num2)
{
    size_t pos = input.find(delimiter);
    if (pos != std::string::npos)
    {
        num1 = std::stoi(input.substr(0, pos));
        num2 = std::stoi(input.substr(pos + 1));
    }
}

void printPlayer(std::vector<Card> &vecdata, int score)
{
    std::cout << "===================" << std::endl;
    std::cout << "your socre: " << score << " your cards:" << std::endl;
    for (const auto &element : vecdata)
    {
        std::cout << element << " | ";
    }
    std::cout << std::endl;
    std::cout << "===================" << std::endl;
}

void printDealer(std::vector<Card> &vecdata, int score)
{
    std::cout << "===================" << std::endl;
    std::cout << "dealer socre: " << score << " dealer cards:" << std::endl;
    for (const auto &element : vecdata)
    {
        std::cout << element << " | ";
    }
    std::cout << std::endl;
    std::cout << "====================" << std::endl;
}

int main(int argc, char *argv[])
{
    std::vector<Card> playerhand{}, dealerhand{};
    std::string login, password, str;
    char option{};
    int val{}, balance{}, cardsymbol{}, figure{}, playerscore{}, dealerscore{};
    bool ingame{};
    std::string cardsymb[13]{"A", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "J", "Q", "K"};

    std::string fig[4]{"diamonds", "hearts", "clubs", "spades"};
    const char *server_ip = argv[1];
    int sock{};
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    // creating a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "error while mkaing a socket\n";
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0)
    {
        std::cerr << "wrong ip\n";
        return -1;
    }

    // server connection
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "connection error\n";
        return -1;
    }

    // get server udp port
    readData(sock, buffer, BUFFER_SIZE);
    short int udpport = std::stoi(buffer);

    std::cout << "Connected to the server, login(l) or register(r): \n";
    std::cin >> option;

    // sending message to server
    switch (option)
    {
    case 'l':
        buffer[0] = 'l';
        sendData(sock, buffer, BUFFER_SIZE);

        std::cout << "Insert login: " << std::endl;
        std::cin >> login;
        strncpy(buffer, login.c_str(), BUFFER_SIZE - 1);
        sendData(sock, buffer, BUFFER_SIZE);

        do
        {
            std::cout << "Insert password: " << std::endl;
            std::cin >> password;
            strncpy(buffer, password.c_str(), BUFFER_SIZE - 1);
            sendData(sock, buffer, BUFFER_SIZE);
            readData(sock, buffer, BUFFER_SIZE);
        } while (buffer[0] != 'y');
        break;
    case 'r':
        buffer[0] = 'r';
        sendData(sock, buffer, BUFFER_SIZE);

        std::cout << "Insert login: " << std::endl;
        std::cin >> login;
        strncpy(buffer, login.c_str(), BUFFER_SIZE - 1);
        sendData(sock, buffer, BUFFER_SIZE);

        std::cout << "Insert password: " << std::endl;
        std::cin >> password;
        strncpy(buffer, password.c_str(), BUFFER_SIZE - 1);
        sendData(sock, buffer, BUFFER_SIZE);

        break;
    default:
        std::cout << "Unsupported command" << std::endl;
    }
    // UDP*******************************
    int udpsock;
    struct sockaddr_in servaddr;

    udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsock < 0)
    {
        std::cerr << "UDP socket creation failed!" << std::endl;
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(udpport);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    //****************
    while (true)
    {
        std::cout << "Insert command: " << std::endl;
        std::cin >> option;

        switch (option)
        {
        case 'd':
            buffer[0] = option;
            sendData(sock, buffer, BUFFER_SIZE);
            std::cin >> val;
            sprintf(buffer, "%d", val);
            sendData(sock, buffer, BUFFER_SIZE);
            break;
        case 'w':
            buffer[0] = option;
            sendData(sock, buffer, BUFFER_SIZE);
            readData(sock, buffer, BUFFER_SIZE);
            balance = std::stoi(buffer);
            std::cin >> val;
            while (val > balance)
            {
                std::cout << "Your balance is too low :(" << std::endl;
                std::cin >> val;
            }
            sprintf(buffer, "%d", val);
            sendData(sock, buffer, BUFFER_SIZE);
            break;
        case 'b':
            buffer[0] = option;
            sendData(sock, buffer, BUFFER_SIZE);
            readData(sock, buffer, BUFFER_SIZE);
            std::cout << "Your balance is " << std::stoi(buffer) << std::endl;
            break;
        case 'r':
            buffer[0] = option;
            sendData(sock, buffer, BUFFER_SIZE);
            std::cin >> login;
            strncpy(buffer, login.c_str(), BUFFER_SIZE - 1);
            sendData(sock, buffer, BUFFER_SIZE);
            readData(sock, buffer, BUFFER_SIZE);
            val = std::stoi(buffer);
            std::cout << "Player " << login << " has " << val << " wins!" << std::endl;
            break;
        case 'j':
            buffer[0] = option;
            sendData(sock, buffer, BUFFER_SIZE);
            readData(sock, buffer, BUFFER_SIZE);
            balance = std::stoi(buffer);

            std::cin >> val;
            while (val > balance)
            {
                std::cout << "Your balance is too low :(" << std::endl;
                std::cin >> val;
            }
            sprintf(buffer, "%d", val);
            sendData(sock, buffer, BUFFER_SIZE);

            // dummy send
            sendData(udpsock, buffer, BUFFER_SIZE, &servaddr);
            // end

            ingame = true;
            {
                readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                str = std::string(buffer);
                readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                dealerscore = std::stoi(buffer);
                splitString(str, 'x', cardsymbol, figure);
                Card card{fig[figure], cardsymb[cardsymbol]};
                dealerhand.push_back(card);
            }
            while (ingame)
            {
                std::cout << "Hit[h] or stay[s]?" << std::endl;
                std::cin >> buffer[0];
                sendData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                if (buffer[0] == 'h')
                {
                    readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                    str = std::string(buffer);
                    readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                    playerscore = std::stoi(buffer);
                    splitString(str, 'x', cardsymbol, figure);
                    Card card{fig[figure], cardsymb[cardsymbol]};
                    playerhand.push_back(card);
                    printDealer(dealerhand, dealerscore);
                    printPlayer(playerhand, playerscore);
                }

                if (buffer[0] == 's')
                {
                    while (dealerscore >= 0 && dealerscore < 17)
                    {
                        readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                        str = std::string(buffer);
                        readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                        dealerscore = std::stoi(buffer);
                        splitString(str, 'x', cardsymbol, figure);
                        Card card{fig[figure], cardsymb[cardsymbol]};
                        dealerhand.push_back(card);
                    }
                    printDealer(dealerhand, dealerscore);
                    printPlayer(playerhand, playerscore);
                }
                readData(udpsock, buffer, BUFFER_SIZE, &servaddr);
                val = std::stoi(buffer);
                std::cout << val << std::endl;
                if (val == 1234)
                {
                    ingame = false;
                    std::cout << "LOST :(" << std::endl;
                    playerhand.clear();
                    dealerhand.clear();
                }
                else if (val == 5678)
                {
                    ingame = false;
                    std::cout << "Win :>" << std::endl;
                    playerhand.clear();
                    dealerhand.clear();
                }
            }
            break;
        case 's':
            buffer[0] = option;
            sendData(sock, buffer, BUFFER_SIZE);
            close(sock);
            close(udpsock);
            return 0;
        default:
            std::cout << "Unsupported command" << std::endl;
        }
    }
}
