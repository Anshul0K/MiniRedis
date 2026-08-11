#include "router/RouterServer.h"
#include "protocol/RESPParser.h"
#include "protocol/RESPEncoder.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cerrno>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <vector>

RouterServer::RouterServer(
    int port,
    const std::vector<int>& nodePorts
)
    : serverSocket(-1),
      port(port),
      running(true),
      nodePorts(nodePorts),
      shardRouter(nodePorts)
{
}

void RouterServer::startShardNodes()
{
    std::cout << "Starting shard nodes..." << std::endl;

    for (int nodePort : nodePorts)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            std::cout
                << "Failed to fork shard on port "
                << nodePort
                << std::endl;

            continue;
        }

        // Child process
        if (pid == 0)
        {
            std::string portString =
                std::to_string(nodePort);

            execl(
                "./build/MiniRedis",
                "MiniRedis",
                portString.c_str(),
                static_cast<char*>(nullptr)
            );

            // Only reached if execl fails
            std::cerr
                << "Failed to start MiniRedis on port "
                << nodePort
                << std::endl;

            _exit(1);
        }

        // Parent process
        std::cout
            << "Started shard on port "
            << nodePort
            << " (PID "
            << pid
            << ")"
            << std::endl;
    }
}

void RouterServer::start()
{
    startShardNodes();

    
    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (serverSocket == -1)
    {
        std::cout << "Failed to create router socket"
                  << std::endl;
        return;
    }

    int opt = 1;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)
        ) == -1)
    {
        std::cout << "Router bind failed"
                  << std::endl;

        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 10) == -1)
    {
        std::cout << "Router listen failed"
                  << std::endl;

        close(serverSocket);
        return;
    }

    std::cout << "Router listening on port "
              << port
              << std::endl;

    while (running)
    {
        int clientSocket =
            accept(serverSocket, nullptr, nullptr);

        if (clientSocket == -1)
        {
            std::cout << "Router accept failed"
                      << std::endl;
            continue;
        }

        std::cout << "Client connected to router"
                  << std::endl;

        std::thread(
            &RouterServer::handleClient,
            this,
            clientSocket
        ).detach();
    }

    close(serverSocket);
}

void RouterServer::handleClient(int clientSocket)
{
    RESPParser parser;

    char buffer[4096];
    std::string inputBuffer;

    while (true)
    {
        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer),
            0
        );

        if (bytesReceived <= 0)
            break;

        inputBuffer.append(buffer, bytesReceived);

        while (true)
        {
            // Keep a copy because parser removes
            // the parsed command from inputBuffer.
            std::string originalBuffer = inputBuffer;

            std::vector<std::string> tokens;

            ParseResult result =
                parser.tryParse(
                    inputBuffer,
                    tokens
                );

            if (result == ParseResult::INCOMPLETE)
                break;

            if (result == ParseResult::ERROR)
            {
                close(clientSocket);
                return;
            }

            if (tokens.empty())
                continue;

            // Current MiniRedis commands have
            // the key at position 1.
            if (tokens.size() < 2)
            {
                close(clientSocket);
                return;
            }

            std::string key = tokens[1];

            // Find the shard responsible for this key.
            int nodePort =
                shardRouter.getNode(key);

            if (nodePort == -1)
            {
                close(clientSocket);
                return;
            }

            std::cout
                << "Key '" << key
                << "' routed to port "
                << nodePort
                << std::endl;

            // Connect to selected MiniRedis node.
            int nodeSocket =
                connectToNode(nodePort);

            if (nodeSocket == -1)
            {
                std::cout
                    << "Failed to connect to node "
                    << nodePort
                    << std::endl;

                close(clientSocket);
                return;
            }

            // Find how many bytes belonged to this command.
            size_t commandSize =
                parser.lastParsedBytes();

            // Extract exactly that RESP command.
            std::string command =
                originalBuffer.substr(
                    0,
                    commandSize
                );

            // Forward command to shard.
            if (send(
                    nodeSocket,
                    command.c_str(),
                    command.size(),
                    0
                ) == -1)
            {
                close(nodeSocket);
                close(clientSocket);
                return;
            }

            // Receive response from shard.
            char responseBuffer[4096];

            int responseBytes = recv(
                nodeSocket,
                responseBuffer,
                sizeof(responseBuffer),
                0
            );

            if (responseBytes <= 0)
            {
                close(nodeSocket);
                close(clientSocket);
                return;
            }

            // Forward shard response to client.
            if (send(
                    clientSocket,
                    responseBuffer,
                    responseBytes,
                    0
                ) == -1)
            {
                close(nodeSocket);
                close(clientSocket);
                return;
            }

            close(nodeSocket);
        }
    }

    close(clientSocket);
}

int RouterServer::connectToNode(int nodePort)
{
    int socketFd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (socketFd == -1)
        return -1;

    sockaddr_in nodeAddress{};

    nodeAddress.sin_family = AF_INET;
    nodeAddress.sin_port = htons(nodePort);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &nodeAddress.sin_addr
    );

    if (connect(
            socketFd,
            reinterpret_cast<sockaddr*>(&nodeAddress),
            sizeof(nodeAddress)
        ) == -1)
    {
        close(socketFd);
        return -1;
    }

    return socketFd;
}