#pragma once

#include <string>
#include <vector>

class CommandParser
{
public:
    std::vector<std::string> parse(const std::string& command);
};