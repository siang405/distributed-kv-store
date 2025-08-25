#pragma once
#include <string>
#include <unordered_map>
#include "hash_ring.hpp"
#include "node.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct NodeInfo {
    string host;
    int port;
};

class Coordinator {
private:
    HashRing ring;
    unordered_map<string, NodeInfo> nodes;

    // 👉 靜態 helper，因為不依賴 Coordinator 狀態
    static string send_request(const string& host, int port, const json& j);

public:
    Coordinator();

    void add_node(const string& id, int port);
    void remove_node(const string& id);
    void put(const string& key, const string& value);
    string get(const string& key);
    void del(const string& key);
    void show_stats() const;
    void show_nodes() const;
};
