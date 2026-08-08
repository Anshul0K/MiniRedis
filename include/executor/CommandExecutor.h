#pragma once

#include "storage/Database.h"
#include "persistence/AOFManager.h"

#include <string>
#include <vector>

class CommandExecutor
{
public:
    CommandExecutor(Database& database, AOFManager& aofManager);

    std::string execute(const std::vector<std::string>& tokens);

private:
    Database& database;
    AOFManager& aofManager;
};