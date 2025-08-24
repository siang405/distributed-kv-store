#pragma once
#include <map>
#include <string>
#include <vector>
using namespace std;

class HashRing {
public:
    HashRing(int replicas = 3);

    void add_node(const string& node_id);
    void remove_node(const string& node_id);
    string get_node(const string& key);

    void show_ring();

private:
    int replicas;
    map<size_t, string> ring; // hash -> node_id
    size_t hash_fn(const string& key);
};
