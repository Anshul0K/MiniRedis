#pragma once

#include <map>
#include <string>
#include <vector>

class ShardRouter
{
public:
    explicit ShardRouter(const std::vector<int>& ports);

    int getNode(const std::string& key) const;

private:
    std::map<size_t, int> hashRing;

    void addNode(int port);
};