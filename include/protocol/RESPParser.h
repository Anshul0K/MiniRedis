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

    size_t lastParsedBytes() const;

private:
    size_t lastParsedSize = 0;
};