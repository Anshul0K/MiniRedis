#include "executor/CommandExecutor.h"

CommandExecutor::CommandExecutor(Database& database, AOFManager& aofManager)
    : database(database), aofManager(aofManager)
{
}

std::string CommandExecutor::execute(const std::vector<std::string>& tokens)
{
    if (tokens.empty())
        return "ERROR: Empty command\n";

    if (tokens[0] == "SET")
    {
        if (tokens.size() != 3)
            return "ERROR: Usage SET <key> <value>\n";

        database.set(tokens[1], tokens[2]);

        aofManager.append("SET " + tokens[1] + " " + tokens[2]);

        return "OK\n";
    }

    if (tokens[0] == "GET")
    {
        if (tokens.size() != 2)
            return "ERROR: Usage GET <key>\n";

        return database.get(tokens[1]) + "\n";
    }

    if (tokens[0] == "DEL")
    {
        if (tokens.size() != 2)
            return "ERROR: Usage DEL <key>\n";

        bool deleted = database.del(tokens[1]);

        if (deleted)
        {
            aofManager.append("DEL " + tokens[1]);
            return "OK\n";
        }

        return "(nil)\n";
    }

    if (tokens[0] == "SNAPSHOT")
    {
        if (tokens.size() != 1)
            return "ERROR: Usage SNAPSHOT\n";

        aofManager.createSnapshot(database);
        return "OK\n";
    }

    return "ERROR: Unknown command\n";
}