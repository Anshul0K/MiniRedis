#pragma once

#include "sharding/ShardRouter.h"

#include <atomic>
#include <vector>

class RouterServer
{
public:
    RouterServer(
        int port,
        const std::vector<int>& nodePorts
    );

    void start();

private:
    void handleClient(int clientSocket);
    int connectToNode(int nodePort);

    int serverSocket;
    int port;

    std::atomic<bool> running;

    ShardRouter shardRouter;
};