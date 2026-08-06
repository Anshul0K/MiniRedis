#pragma once

#include <netinet/in.h>
#include "storage/Database.h"

class Server
{
public:
    Server();
    void start();

private:
    void handleClient(int clientSocket);

    int serverSocket;
    Database database;
    sockaddr_in serverAddress;
};