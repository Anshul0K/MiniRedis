#pragma once

#include <string>
#include <vector>

class Config
{
public:
    static Config& getInstance();

    bool load(const std::string& filename);

    int routerPort;
    std::vector<int> shardPorts;
    int snapshotInterval;
    size_t maxKeys;

private:
    Config();

    void parseLine(const std::string& line);
};