#pragma once

#include "storage/Database.h"
#include "persistence/AOFManager.h"

#include <string>
#include <vector>

enum class ResponseType
{
    SIMPLE_STRING,
    BULK_STRING,
    NULL_VALUE,
    ERROR
};

struct CommandResponse
{
    ResponseType type;
    std::string value;
};

class CommandExecutor
{
public:
    CommandExecutor(Database& database, AOFManager& aofManager);

    CommandResponse execute(
        const std::vector<std::string>& tokens
    );

private:
    Database& database;
    AOFManager& aofManager;
};