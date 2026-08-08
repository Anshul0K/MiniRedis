#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>

class Database
{
public:
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const std::string& value, long long ttl);

    std::string get(const std::string& key);
    std::unordered_map<std::string, std::string> getAll();
    std::unordered_map<std::string, std::pair<std::string, long long>> getSnapshotData();

    bool del(const std::string& key);
    long long ttl(const std::string& key);

private:
    std::unordered_map<std::string, std::string> data;
    std::unordered_map<std::string, long long> expiry;

    mutable std::shared_mutex mutex;
};