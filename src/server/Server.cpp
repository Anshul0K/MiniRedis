#include "server/Server.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#include "parser/CommandParser.h"
#include "executor/CommandExecutor.h"

Server::Server()
    : serverSocket(-1)
{
}

void Server::start()
{
    std::cout << "MiniRedis Server Starting..." << std::endl;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cout << "Failed to create socket" << std::endl;
        return;
    }
    std::cout << "Socket created successfully" << std::endl;

    // Configure address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(6379);

    // Bind
    if (::bind(serverSocket,
               reinterpret_cast<sockaddr*>(&serverAddress),
               sizeof(serverAddress)) == -1)
    {
        std::cout << "Bind failed" << std::endl;
        close(serverSocket);
        return;
    }
    std::cout << "Bind successful" << std::endl;

    // Listen
    if (listen(serverSocket, 5) == -1)
    {
        std::cout << "Listen failed" << std::endl;
        close(serverSocket);
        return;
    }

    std::cout << "Listening on port 6379..." << std::endl;
    std::cout << "Waiting for client..." << std::endl;

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    if (clientSocket == -1)
    {
        std::cout << "Accept failed" << std::endl;
        close(serverSocket);
        return;
    }

    std::cout << "Client connected!" << std::endl;

    CommandParser parser;
    CommandExecutor executor;

    char buffer[1024];

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::string command(buffer);

        auto tokens = parser.parse(command);

        std::string response = executor.execute(tokens);

        if (send(clientSocket,
                 response.c_str(),
                 response.size(),
                 0) == -1)
        {
            std::cout << "Failed to send response" << std::endl;
            break;
        }
    }

    close(clientSocket);
    close(serverSocket);
}