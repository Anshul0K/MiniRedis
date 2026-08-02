#include "parser/CommandParser.h"

#include <sstream>

std::vector<std::string> CommandParser::parse(const std::string& command)
{
    std::stringstream ss(command);

    std::vector<std::string> tokens;
    std::string token;

    while (ss >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}