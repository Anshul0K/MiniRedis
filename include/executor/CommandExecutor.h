#pragma once

#include "storage/Database.h"

#include <string>
#include <vector>

class CommandExecutor
{
public:
    std::string execute(const std::vector<std::string>& tokens);

private:
    Database database;
};