#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include "game_functions.hpp"
#include <csignal>
#include <sys/wait.h>

#define PORT 2000
#define PORT_UDP 2001
#define BUFF_SIZE 1024

ssize_t sendData(int socket, const char *data, size_t length, struct sockaddr_in *destAddr = nullptr, int address = 0)
{
    if (destAddr == nullptr)
    {
        return send(socket, data, length, 0); // TCP
    }
    else
    {
        // destAddr->sin_addr.s_addr = address;
        return sendto(socket, data, length, 0, (const struct sockaddr *)destAddr, sizeof(*destAddr)); // UDP
    }
}

ssize_t readData(int socket, char *buffer, size_t bufferSize, struct sockaddr_in *srcAddr = nullptr, int address = 0)
{

    if (srcAddr == nullptr)
    {
        return read(socket, buffer, bufferSize); // TCP
    }
    else
    {
        socklen_t addrlen = sizeof(*srcAddr);
        // srcAddr->sin_addr.s_addr = address;
        return recvfrom(socket, buffer, bufferSize, 0, (struct sockaddr *)srcAddr, &addrlen); // UDP
    }
}

void handlectrlc(int sig)
{
    std::cout << "\nCtrl+c!!!\n"
              << std::endl;
    exit(0);
}

void childTerm(int sig)
{
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0)
    {
        std::cout << "Child process terminated" << std::endl;
    }
}

int main()
{
    std::cout << "Server listening on TCP port " << PORT << " and UDP port " << PORT_UDP << " - 30000" << std::endl;
    // TCP initialization ***************************************
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFF_SIZE] = {0};
    int playercount{};
    short int udpport{2001};
    signal(SIGCHLD, childTerm);
    signal(SIGINT, handlectrlc);
    // Creating TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configuring server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding address to socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accepting connections ************************************
    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            continue;
        }
        std::cout << "New connection" << std::endl;

        uint32_t clientIP = address.sin_addr.s_addr;
        udpport++;
        if (udpport == 30000)
            udpport = 2001;
        sprintf(buffer, "%d", udpport);

        // Forking a child process to handle the client
        pid_t pid = fork();
        if (pid == 0)
        { // Child process
            std::string login, password, cardinfo;
            srand(time(0));
            struct sockaddr_in cliaddr;
            bool loop{true}, gameinprogress{false};
            int val{}, unsuppcounter{};
            char commands = 0;
            std::vector<Player> regdata{};
            Player dealer{"dealer", "xxxxxx", 11, 11};
            regdata.emplace_back(dealer);

            // UDP Setup ************************************************
            int udpsock;
            struct sockaddr_in udpAddr;

            if ((udpsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            {
                std::cerr << "UDP socket creation failed!" << std::endl;
                return -1;
            }
            udpAddr.sin_family = AF_INET;
            udpAddr.sin_addr.s_addr = INADDR_ANY;
            udpAddr.sin_port = htons(udpport);
            if (bind(udpsock, (struct sockaddr *)&udpAddr, sizeof(udpAddr)) < 0)
            {
                std::cerr << "UDP bind failed!" << std::endl;
                return -1;
            }
            // end of init udp
            sendData(new_socket, buffer, BUFF_SIZE);
            read(new_socket, buffer, BUFF_SIZE);

            switch (buffer[0])
            {
            case 'r':
            {
                std::cout << "Registering new player" << std::endl;
                readData(new_socket, buffer, BUFF_SIZE);
                login = std::string(buffer);
                readData(new_socket, buffer, BUFF_SIZE);
                password = std::string(buffer);
                playerLogin(regdata, playerRegister(login, password), password);
                break;
            }
            case 'l':
            {
                std::cout << "Logging in" << std::endl;
                readData(new_socket, buffer, BUFF_SIZE);
                login = std::string(buffer);
                readData(new_socket, buffer, BUFF_SIZE);
                password = std::string(buffer);

                while (!playerLogin(regdata, login, password))
                {
                    buffer[0] = 'n';
                    sendData(new_socket, buffer, BUFF_SIZE);
                    readData(new_socket, buffer, BUFF_SIZE);
                    password = std::string(buffer);
                }
                buffer[0] = 'y';
                sendData(new_socket, buffer, BUFF_SIZE);
                break;
            }
            default:
                std::cout << "Unsupported command!!!" << std::endl;
            }

            while (loop)
            {
                std::cout << "Command received from id: " << clientIP << " on port " << udpport << std::endl;

                // Reading from TCP
                readData(new_socket, buffer, BUFF_SIZE);
                switch (buffer[0])
                {
                case 'd':
                {
                    readData(new_socket, buffer, BUFF_SIZE);
                    val = std::stoi(buffer);
                    regdata[1].deposit(val);
                    editFile(regdata[1].getLogin(), val, 0);
                    break;
                }
                case 'w':
                {
                    sprintf(buffer, "%d", regdata[1].getWallet());
                    sendData(new_socket, buffer, BUFF_SIZE);
                    readData(new_socket, buffer, BUFF_SIZE);
                    val = std::stoi(buffer);
                    regdata[1].withdraw(val);
                    editFile(regdata[1].getLogin(), val, 1);
                    break;
                }
                case 'b':
                {
                    sprintf(buffer, "%d", regdata[1].getWallet());
                    sendData(new_socket, buffer, BUFF_SIZE);
                    break;
                }
                case 'r':
                {
                    readData(new_socket, buffer, BUFF_SIZE);
                    login = std::string(buffer);
                    sprintf(buffer, "%d", ranking(login));
                    sendData(new_socket, buffer, BUFF_SIZE);
                    break;
                }
                case 'j':
                {
                    sprintf(buffer, "%d", regdata[1].getWallet());
                    sendData(new_socket, buffer, BUFF_SIZE);
                    readData(new_socket, buffer, BUFF_SIZE);
                    val = std::stoi(buffer);

                    // Dummy read to get client address
                    readData(udpsock, buffer, BUFF_SIZE, &cliaddr);
                    // end

                    regdata[1].setScore(0);
                    regdata[0].setScore(0);
                    cardinfo = regdata[0].drawCard();
                    strncpy(buffer, cardinfo.c_str(), BUFF_SIZE - 1);
                    sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                    sprintf(buffer, "%d", regdata[0].getScore());
                    sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);

                    while (regdata[1].getScore() != -1)
                    {

                        readData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);

                        if (buffer[0] == 'h')
                        {
                            playgame(regdata, buffer[0], cardinfo);
                            strncpy(buffer, cardinfo.c_str(), BUFF_SIZE - 1);
                            sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                            sprintf(buffer, "%d", regdata[1].getScore());
                            sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                        }
                        if (buffer[0] == 's')
                        {
                            while (regdata[0].getScore() >= 0 && regdata[0].getScore() < 17)
                            {
                                playgame(regdata, buffer[0], cardinfo);
                                strncpy(buffer, cardinfo.c_str(), BUFF_SIZE - 1);
                                sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                                sprintf(buffer, "%d", regdata[0].getScore());
                                sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                            }
                        }

                        if (regdata[1].getWin() == false && regdata[1].getScore() == -1)
                        {
                            sprintf(buffer, "%d", 1234);
                            sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                            regdata[1].withdraw(val);
                            editFile(regdata[1].getLogin(), val, 1);
                        }
                        else if (regdata[1].getWin() == true && regdata[1].getScore() == -1)
                        {
                            sprintf(buffer, "%d", 5678);
                            sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                            regdata[1].setWin(false);
                            regdata[1].deposit(val);
                            editFile(regdata[1].getLogin(), val, 0);
                        }
                        else
                        {
                            sprintf(buffer, "%d", 4321);
                            sendData(udpsock, buffer, BUFF_SIZE, &cliaddr, clientIP);
                        }
                    }
                    break;
                }
                case 's':
                {
                    loop = 0;
                    close(udpsock);
                    exit(0);
                    break;
                }
                default:
                    std::cout << "Unsupported command!!!!" << std::endl;
                    unsuppcounter++;
                    if (unsuppcounter > 2)
                        exit(0);
                }
            }
            exit(0); // Exit child process
        }
    }
    close(new_socket);
    close(server_fd);
    return 0;
}
