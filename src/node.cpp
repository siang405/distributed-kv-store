#include "node.hpp"
#include <iostream>
using namespace std;

Node::Node(const string& id) : id(id) {}

void Node::put(const string& key, const string& value) {
    store[key] = value;
    cout << "[" << id << "] PUT " << key << " = " << value << "\n";
}

string Node::get(const string& key) {
    if (store.find(key) != store.end()) {
        return store.at(key);
    }
    return "[NOT FOUND]";
}

void Node::del(const string& key) {
    store.erase(key);
}

string Node::get_id() const {
    return id;
}

size_t Node::get_size() const {
    return store.size();
}

vector<pair<string,string>> Node::get_all() const {
    vector<pair<string,string>> all;
    for (auto& kv : store) {
        all.push_back(kv);
    }
    return all;
}
