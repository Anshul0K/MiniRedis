#include "router/RouterServer.h"
#include "config/Config.h"
#include <iostream>

int main()
{
    Config& config = Config::getInstance();

    if (!config.load(".env"))
    {
        std::cout << "Failed to load .env" << std::endl;
        return 1;
    }

    RouterServer router(
        config.routerPort,
        config.shardPorts,
        config.snapshotInterval
    );

    router.start();

    return 0;
}