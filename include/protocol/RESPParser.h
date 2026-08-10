#pragma once

#include <string>
#include <vector>

enum class ParseResult
{
    COMPLETE,
    INCOMPLETE,
    ERROR
};

class RESPParser
{
public:
    ParseResult tryParse(
        std::string& buffer,
        std::vector<std::string>& tokens
    );
};