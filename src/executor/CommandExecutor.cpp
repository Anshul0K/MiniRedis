#include "executor/CommandExecutor.h"

CommandExecutor::CommandExecutor(
    Database& database,
    AOFManager& aofManager)
    : database(database),
      aofManager(aofManager)
{
}

CommandResponse CommandExecutor::execute(
    const std::vector<std::string>& tokens)
{
    if (tokens.empty())
    {
        return {
            ResponseType::ERROR,
            "Empty command"
        };
    }

    // -------------------------
    // SET
    // -------------------------

    if (tokens[0] == "SET")
    {
        if (tokens.size() != 3)
        {
            return {
                ResponseType::ERROR,
                "Usage: SET <key> <value>"
            };
        }

        database.set(tokens[1], tokens[2]);

        aofManager.append(
            "SET " + tokens[1] + " " + tokens[2]
        );

        return {
            ResponseType::SIMPLE_STRING,
            "OK"
        };
    }

    // -------------------------
    // GET
    // -------------------------

    if (tokens[0] == "GET")
    {
        if (tokens.size() != 2)
        {
            return {
                ResponseType::ERROR,
                "Usage: GET <key>"
            };
        }

        std::string value = database.get(tokens[1]);

        if (value == "(nil)")
        {
            return {
                ResponseType::NULL_VALUE,
                ""
            };
        }

        return {
            ResponseType::BULK_STRING,
            value
        };
    }

    // -------------------------
    // DEL
    // -------------------------

    if (tokens[0] == "DEL")
    {
        if (tokens.size() != 2)
        {
            return {
                ResponseType::ERROR,
                "Usage: DEL <key>"
            };
        }

        bool deleted = database.del(tokens[1]);

        if (deleted)
        {
            aofManager.append(
                "DEL " + tokens[1]
            );

            return {
                ResponseType::SIMPLE_STRING,
                "OK"
            };
        }

        return {
            ResponseType::NULL_VALUE,
            ""
        };
    }

    return {
        ResponseType::ERROR,
        "Unknown command"
    };
}