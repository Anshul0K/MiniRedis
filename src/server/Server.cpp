#include "server/Server.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>

#include "executor/CommandExecutor.h"
#include "persistence/AOFManager.h"
#include "protocol/RESPParser.h"
#include "protocol/RESPEncoder.h"

Server::Server(int port)
    : serverSocket(-1),
      running(true),
      port(port),
      aofManager(port)
{
}

void Server::handleClient(int clientSocket)
{
    RESPParser parser;
    CommandExecutor executor(database, aofManager);

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
        {
            std::cout << "Client disconnected"
                      << std::endl;
            break;
        }

        // Add received bytes to persistent buffer
        inputBuffer.append(buffer, bytesReceived);

        // One recv() can contain multiple commands
        while (true)
        {
            std::vector<std::string> tokens;

            ParseResult result =
                parser.tryParse(
                    inputBuffer,
                    tokens
                );

            // Command is incomplete.
            // Wait for the next recv().
            if (result == ParseResult::INCOMPLETE)
                break;

            // Invalid RESP sent by client.
            if (result == ParseResult::ERROR)
            {
                std::string response =
                    RESPEncoder::encodeError(
                        "Protocol error"
                    );

                send(
                    clientSocket,
                    response.c_str(),
                    response.size(),
                    0
                );

                close(clientSocket);
                return;
            }

            // Complete command
            if (tokens.empty())
                continue;

            CommandResponse commandResponse =
                executor.execute(tokens);

            std::string response;

            switch (commandResponse.type)
            {
                case ResponseType::SIMPLE_STRING:
                    response =
                        RESPEncoder::encodeSimpleString(
                            commandResponse.value
                        );
                    break;

                case ResponseType::BULK_STRING:
                    response =
                        RESPEncoder::encodeBulkString(
                            commandResponse.value
                        );
                    break;

                case ResponseType::NULL_VALUE:
                    response =
                        RESPEncoder::encodeNull();
                    break;

                case ResponseType::ERROR:
                    response =
                        RESPEncoder::encodeError(
                            commandResponse.value
                        );
                    break;
            }

            if (send(
                    clientSocket,
                    response.c_str(),
                    response.size(),
                    0
                ) == -1)
            {
                std::cout
                    << "Failed to send response"
                    << std::endl;

                close(clientSocket);
                return;
            }
        }
    }

    close(clientSocket);
}

void Server::start()
{
    std::cout << "MiniRedis Server Starting..." << std::endl;
    aofManager.load(database);

    snapshotThread = std::thread(&Server::snapshotLoop, this);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1)
    {
        std::cout << "Failed to create socket" << std::endl;
        return;
    }

    std::cout << "Socket created successfully" << std::endl;

    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (::bind(serverSocket,
               reinterpret_cast<sockaddr*>(&serverAddress),
               sizeof(serverAddress)) == -1)
    {
        std::cout << "Bind failed" << std::endl;
        close(serverSocket);
        return;
    }

    std::cout << "Bind successful" << std::endl;

    if (listen(serverSocket, 5) == -1)
    {
        std::cout << "Listen failed" << std::endl;
        close(serverSocket);
        return;
    }

    std::cout << "Listening on port "<< port<< "..."<< std::endl;

    while (true)
    {
        std::cout << "Waiting for client..." << std::endl;

        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket == -1)
        {
            std::cout << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        std::thread(&Server::handleClient, this, clientSocket).detach();
    }

    close(serverSocket);
}

void Server::snapshotLoop()
{
    while (running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(300));

        if (running)
        {
            aofManager.createSnapshot(database);
            std::cout << "Automatic snapshot created" << std::endl;
        }
    }
}