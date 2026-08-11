#include "server/Server.h"

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./MiniRedis <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    Server server(port);
    server.start();

    return 0;
}