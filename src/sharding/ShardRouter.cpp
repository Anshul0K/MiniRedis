#include "sharding/ShardRouter.h"

#include <functional>

ShardRouter::ShardRouter(const std::vector<int>& ports)
{
    for (int port : ports)
    {
        this->addNode(port);
    }
}

void ShardRouter::addNode(int port)
{
    size_t hashValue = std::hash<std::string>{}(
        "node:" + std::to_string(port)
    );

    hashRing[hashValue] = port;
}

int ShardRouter::getNode(const std::string& key) const
{
    if (hashRing.empty())
        return -1;

    size_t hashValue = std::hash<std::string>{}(key);

    auto it = hashRing.lower_bound(hashValue);

    if (it == hashRing.end())
        it = hashRing.begin();

    return it->second;
}