#include "storage/Database.h"

#include <chrono>
#include <mutex>

void Database::set(const std::string& key, const std::string& value)
{
    std::unique_lock lock(mutex);

    data[key] = value;
    expiry.erase(key);
}

void Database::set(const std::string& key, const std::string& value, long long ttl)
{
    std::unique_lock lock(mutex);

    data[key] = value;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    expiry[key] = timestamp + ttl;
}

std::string Database::get(const std::string& key)
{
    std::unique_lock lock(mutex);

    auto it = data.find(key);

    if (it == data.end())
        return "(nil)";

    auto exp = expiry.find(key);

    if (exp != expiry.end())
    {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        ).count();

        if (timestamp >= exp->second)
        {
            data.erase(it);
            expiry.erase(exp);
            return "(nil)";
        }
    }

    return it->second;
}

bool Database::del(const std::string& key)
{
    std::unique_lock lock(mutex);

    expiry.erase(key);
    return data.erase(key);
}

long long Database::ttl(const std::string& key)
{
    std::unique_lock lock(mutex);

    if (data.find(key) == data.end())
        return -2;

    auto exp = expiry.find(key);

    if (exp == expiry.end())
        return -1;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    if (timestamp >= exp->second)
    {
        data.erase(key);
        expiry.erase(exp);
        return -2;
    }

    return exp->second - timestamp;
}

std::unordered_map<std::string, std::string> Database::getAll()
{
    std::unique_lock lock(mutex);

    std::unordered_map<std::string, std::string> result;

    for (auto it = data.begin(); it != data.end();)
    {
        auto exp = expiry.find(it->first);

        if (exp != expiry.end())
        {
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()
            ).count();

            if (timestamp >= exp->second)
            {
                expiry.erase(exp);
                it = data.erase(it);
                continue;
            }
        }

        result[it->first] = it->second;
        ++it;
    }

    return result;
}

std::unordered_map<std::string, std::pair<std::string, long long>> Database::getSnapshotData()
{
    std::unique_lock lock(mutex);

    std::unordered_map<std::string, std::pair<std::string, long long>> result;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    for (auto it = data.begin(); it != data.end();)
    {
        auto exp = expiry.find(it->first);

        if (exp != expiry.end() && timestamp >= exp->second)
        {
            expiry.erase(exp);
            it = data.erase(it);
            continue;
        }

        long long expiration = -1;

        if (exp != expiry.end())
            expiration = exp->second;

        result[it->first] = {it->second, expiration};

        ++it;
    }

    return result;
}