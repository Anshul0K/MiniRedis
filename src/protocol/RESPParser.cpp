#include "protocol/RESPParser.h"

#include <string>

ParseResult RESPParser::tryParse(
    std::string& buffer,
    std::vector<std::string>& tokens)
{
    tokens.clear();

    size_t pos = 0;

    // Helper to read a line ending in \r\n
    auto readLine = [&](std::string& line) -> ParseResult
    {
        size_t end = buffer.find("\r\n", pos);

        if (end == std::string::npos)
            return ParseResult::INCOMPLETE;

        line = buffer.substr(pos, end - pos);

        pos = end + 2;

        return ParseResult::COMPLETE;
    };

    std::string line;

    // -----------------------------
    // Read *<number of arguments>
    // -----------------------------

    ParseResult result = readLine(line);

    if (result == ParseResult::INCOMPLETE)
        return ParseResult::INCOMPLETE;

    if (line.empty() || line[0] != '*')
        return ParseResult::ERROR;

    int argumentCount;

    try
    {
        argumentCount = std::stoi(line.substr(1));
    }
    catch (...)
    {
        return ParseResult::ERROR;
    }

    if (argumentCount < 0)
        return ParseResult::ERROR;

    // -----------------------------
    // Read each argument
    // -----------------------------

    for (int i = 0; i < argumentCount; i++)
    {
        // Read $<length>
        result = readLine(line);

        if (result == ParseResult::INCOMPLETE)
            return ParseResult::INCOMPLETE;

        if (line.empty() || line[0] != '$')
            return ParseResult::ERROR;

        int length;

        try
        {
            length = std::stoi(line.substr(1));
        }
        catch (...)
        {
            return ParseResult::ERROR;
        }

        if (length < 0)
            return ParseResult::ERROR;

        // Check whether the complete argument
        // and its trailing \r\n have arrived.
        if (buffer.size() < pos + length + 2)
            return ParseResult::INCOMPLETE;

        // Extract argument
        std::string argument =
            buffer.substr(pos, length);

        pos += length;

        // Argument must be followed by \r\n
        if (buffer.substr(pos, 2) != "\r\n")
            return ParseResult::ERROR;

        pos += 2;

        tokens.push_back(argument);
    }

    // -----------------------------
    // Successfully parsed command
    // -----------------------------

    // Remove only the command we consumed.
    // Remember how many bytes this command used.
    lastParsedSize = pos;

    // Remove only the command we consumed.
    buffer.erase(0, pos);

    return ParseResult::COMPLETE;
}

size_t RESPParser::lastParsedBytes() const
{
    return lastParsedSize;
}