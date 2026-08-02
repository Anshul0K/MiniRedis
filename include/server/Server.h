#pragma once
#include <netinet/in.h>

class Server
{
public:
    Server();

    void start();

private:
    int serverSocket;
    sockaddr_in serverAddress;
};