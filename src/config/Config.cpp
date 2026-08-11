#include "config/Config.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

Config::Config()
    : routerPort(7000),
      snapshotInterval(300),
      maxKeys(1000)
{
}

Config& Config::getInstance()
{
    static Config instance;
    return instance;
}

bool Config::load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        parseLine(line);
    }

    return true;
}

void Config::parseLine(const std::string& line)
{
    if (line.empty() || line[0] == '#')
        return;

    size_t pos = line.find('=');

    if (pos == std::string::npos)
        return;

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);


    if (key == "ROUTER_PORT")
    {
        routerPort = std::stoi(value);
    }
    else if (key == "SHARD_PORTS")
    {
        shardPorts.clear();

        std::stringstream ss(value);
        std::string port;

        while (std::getline(ss, port, ','))
        {
            shardPorts.push_back(std::stoi(port));
        }
    }
    else if (key == "SNAPSHOT_INTERVAL")
    {
        snapshotInterval = std::stoi(value);
    }
    else if (key == "MAX_KEYS")
    {
        maxKeys = std::stoul(value);
    }
}