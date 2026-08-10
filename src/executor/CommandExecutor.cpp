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
    // SET <key> <value>
        if (tokens.size() == 3)
        {
            database.set(tokens[1], tokens[2]);

            aofManager.append(
                "SET " + tokens[1] + " " + tokens[2]
            );

            return {
                ResponseType::SIMPLE_STRING,
                "OK"
            };
        }

        // SET <key> <value> EX <seconds>
        if (tokens.size() == 5)
        {
            if (tokens[3] != "EX")
            {
                return {
                    ResponseType::ERROR,
                    "Syntax error"
                };
            }

            try
            {
                long long ttl = std::stoll(tokens[4]);

                if (ttl <= 0)
                {
                    return {
                        ResponseType::ERROR,
                        "Invalid TTL"
                    };
                }

                database.set(tokens[1], tokens[2], ttl);

                aofManager.append(
                    "SET " + tokens[1] + " " + tokens[2] +
                    " EX " + tokens[4]
                );

                return {
                    ResponseType::SIMPLE_STRING,
                    "OK"
                };
            }
            catch (...)
            {
                return {
                    ResponseType::ERROR,
                    "Invalid TTL"
                };
            }
        }

        return {
            ResponseType::ERROR,
            "Usage: SET <key> <value> [EX <seconds>]"
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