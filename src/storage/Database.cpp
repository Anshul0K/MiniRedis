#include "storage/Database.h"
#include <shared_mutex>
#include <mutex>

using namespace std;

void Database::set(const string& key, const string& value){
    std::unique_lock lock(mutex);
    data[key] = value;
}

string Database::get(const string& key){
    std::shared_lock lock(mutex);
    auto it = data.find(key);
    if(it==data.end()){
        return "(nil)";
    }
    return it->second;
}

bool Database::del(const string& key){
    std::unique_lock lock(mutex);
    return data.erase(key);
}