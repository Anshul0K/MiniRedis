#include "storage/Database.h"

using namespace std;

void Database::set(const string& key, const string& value){
    data[key] = value;
}

string Database::get(const string& key){
    auto it = data.find(key);
    if(it==data.end()){
        return "(nil)";
    }
    return it->second;
}

bool Database::del(const string& key){
    return data.erase(key);
}