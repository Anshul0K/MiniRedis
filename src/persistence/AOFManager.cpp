#include "persistence/AOFManager.h"
#include "storage/Database.h"

#include <fstream>
#include <sstream>
#include <cstdio>
#include <mutex>
#include <iostream>
#include <chrono>

AOFManager::AOFManager()
    : aofPath("data/appendonly.aof"),
      snapshotPath("data/snapshot.rdb")
{
}

void AOFManager::append(const std::string& command)
{
    std::lock_guard<std::mutex> lock(mutex);

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

    auto data = database.getSnapshotData();

    for (const auto& [key, entry] : data)
    {
        const auto& value = entry.first;
        long long expiration = entry.second;

        file << key << '\t'
             << value << '\t'
             << expiration << '\n';
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
        std::string line;

        while (std::getline(snapshot, line))
        {
            std::stringstream ss(line);

            std::string key;
            std::string value;
            long long expiration;

            std::getline(ss, key, '\t');
            std::getline(ss, value, '\t');
            ss >> expiration;

            if (expiration == -1)
            {
                database.set(key, value);
                continue;
            }

            auto now = std::chrono::system_clock::now();

            auto currentTime =
                std::chrono::duration_cast<std::chrono::seconds>(
                    now.time_since_epoch()
                ).count();

            long long remainingTTL = expiration - currentTime;

            if (remainingTTL > 0)
            {
                database.set(key, value, remainingTTL);
            }
        }
    }

    snapshot.close();

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
        std::string option;
        long long ttl;

        ss >> operation >> key;

        if (operation == "SET")
        {
            ss >> value;

            if (ss >> option >> ttl)
            {
                if (option == "EX" && ttl > 0)
                    database.set(key, value, ttl);
            }
            else
            {
                database.set(key, value);
            }
        }
        else if (operation == "DEL")
        {
            database.del(key);
        }
    }
}