#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;


class Database{
public:
    void set(const string& key, const string& value);
    string get(const string& key);
    bool del(const string& key);

private:
    unordered_map<string, string> data;

};
