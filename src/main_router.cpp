#include "router/RouterServer.h"

#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout
            << "Usage: "
            << argv[0]
            << " <router-port>"
            << std::endl;

        return 1;
    }

    int routerPort = std::stoi(argv[1]);

    std::vector<int> nodePorts = {
        6379,
        6380,
        6381
    };

    RouterServer router(
        routerPort,
        nodePorts
    );

    router.start();

    return 0;
}