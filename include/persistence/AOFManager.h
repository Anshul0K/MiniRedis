#pragma once

#include <string>

class Database;

class AOFManager
{
public:
    AOFManager();

    void append(const std::string& command);

    void createSnapshot(Database& database);

    void load(Database& database);

private:
    std::string aofPath;
    std::string snapshotPath;
};