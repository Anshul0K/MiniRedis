#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <shared_mutex>


class Database
{
public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    bool del(const std::string& key);

private:
    std::unordered_map<std::string, std::string> data;
    mutable std::shared_mutex mutex;
};