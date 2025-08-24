#pragma once
#include <string>
#include <unordered_map>
#include <vector>   // << 新增
#include <utility>  // << 新增
using namespace std;

class Node {
public:
    Node(const string& id);

    void put(const string& key, const string& value);
    string get(const string& key);
    void del(const string& key);

    string get_id() const;
    size_t get_size() const;

    vector<pair<string,string>> get_all() const;  // for rebalancing

private:
    string id;
    unordered_map<string,string> store;
};
