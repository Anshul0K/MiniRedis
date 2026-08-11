#pragma once

#include <string>
#include <mutex>

class Database;

class AOFManager
{
public:
    explicit AOFManager(int port);

    void append(const std::string& command);

    void createSnapshot(Database& database);

    void load(Database& database);

private:
    std::string aofPath;
    std::string snapshotPath;
    std::mutex mutex;
};