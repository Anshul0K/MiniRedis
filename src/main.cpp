#include "server/Server.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout
            << "Usage: "
            << argv[0]
            << " <port> <snapshotInterval>"
            << std::endl;

        return 1;
    }

    int port = std::stoi(argv[1]);
    int snapshotInterval = std::stoi(argv[2]);

    Server server(
        port,
        snapshotInterval
    );

    server.start();

    return 0;
}