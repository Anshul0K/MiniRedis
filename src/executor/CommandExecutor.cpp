#include "executor/CommandExecutor.h"

std::string CommandExecutor::execute(const std::vector<std::string>& tokens)
{
    if (tokens.empty())
        return "ERROR: Empty command\n";

    if (tokens[0] == "SET")
    {
        if (tokens.size() != 3)
            return "ERROR: Usage SET <key> <value>\n";

        database.set(tokens[1], tokens[2]);
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

        return database.del(tokens[1]) ? "OK\n" : "(nil)\n";
    }

    return "ERROR: Unknown command\n";
}

CommandExecutor::CommandExecutor(Database& database)
    : database(database)
{
}