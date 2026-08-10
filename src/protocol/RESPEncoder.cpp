#include "protocol/RESPEncoder.h"

std::string RESPEncoder::encodeSimpleString(
    const std::string& value)
{
    return "+" + value + "\r\n";
}

std::string RESPEncoder::encodeBulkString(
    const std::string& value)
{
    return "$" +
           std::to_string(value.size()) +
           "\r\n" +
           value +
           "\r\n";
}

std::string RESPEncoder::encodeNull()
{
    return "$-1\r\n";
}

std::string RESPEncoder::encodeError(
    const std::string& message)
{
    return "-" + message + "\r\n";
}