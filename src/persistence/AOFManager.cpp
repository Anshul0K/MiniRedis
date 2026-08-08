#include "persistence/AOFManager.h"
#include "storage/Database.h"

#include <fstream>
#include <sstream>
#include <cstdio>

AOFManager::AOFManager()
    : aofPath("data/appendonly.aof"),
      snapshotPath("data/snapshot.rdb")
{
}

void AOFManager::append(const std::string& command)
{
    std::ofstream file(aofPath, std::ios::app);

    if (!file.is_open())
        return;

    file << command << '\n';
}

void AOFManager::createSnapshot(Database& database)
{
    std::string tempPath = snapshotPath + ".tmp";

    std::ofstream file(tempPath);

    if (!file.is_open())
        return;

    auto data = database.getAll();

    for (const auto& [key, value] : data)
    {
        file << key << '\t' << value << '\n';
    }

    file.close();

    std::remove(snapshotPath.c_str());
    std::rename(tempPath.c_str(), snapshotPath.c_str());

    std::ofstream aof(aofPath, std::ios::trunc);
}

void AOFManager::load(Database& database)
{
    std::ifstream snapshot(snapshotPath);

    if (snapshot.is_open())
    {
        std::string key;
        std::string value;

        while (std::getline(snapshot, key, '\t') &&
               std::getline(snapshot, value))
        {
            database.set(key, value);
        }
    }

    snapshot.close();

    std::ifstream aof(aofPath);

    if (!aof.is_open())
        return;

    std::string command;

    while (std::getline(aof, command))
    {
        std::stringstream ss(command);

        std::string operation;
        std::string key;
        std::string value;

        ss >> operation >> key;

        if (operation == "SET")
        {
            ss >> value;
            database.set(key, value);
        }
        else if (operation == "DEL")
        {
            database.del(key);
        }
    }
}