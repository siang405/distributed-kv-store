#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "hash_ring.hpp"

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
    unordered_map<string, bool> node_status; // true = UP, false = DOWN
    bool running = true;

    static string send_request(const string& host, int port, const json& j);

public:
    Coordinator();
    ~Coordinator();

    void add_node(const string& id, int port);
    void remove_node(const string& id);
    void put(const string& key, const string& value);
    string get(const string& key);
    void del(const string& key);
    void show_nodes() const;
    void show_stats() const;

    // replication
    int replicaN = 1;
    void set_replica(int n) {
        replicaN = max(1, n);
        cout << "[Coordinator] Replica set to " << replicaN << endl;
    }
    vector<string> get_replica_nodes(const string& key) const {
        return ring.get_nodes(key, replicaN);
    }

    // heartbeat
    void start_heartbeat();
    void stop();

    void rebalance();

};
