#pragma once

#include "sharding/ShardRouter.h"

#include <atomic>
#include <vector>
#include <sys/types.h>

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

    void startShardNodes();

    int serverSocket;
    int port;

    std::atomic<bool> running;

    std::vector<int> nodePorts;

    ShardRouter shardRouter;
};