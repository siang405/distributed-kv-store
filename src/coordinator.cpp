#include "coordinator.hpp"
#include <iostream>
using namespace std;

Coordinator::Coordinator() : ring(3) {}

void Coordinator::add_node(const string& node_id) {
    if (nodes.count(node_id)) {
        cout << "Node already exists: " << node_id << "\n";
        return;
    }
    nodes[node_id] = make_shared<Node>(node_id);
    ring.add_node(node_id);

    cout << "Rebalancing after adding node " << node_id << "...\n";

    // 遍歷所有 key，檢查是否要搬到新 node
    for (auto& [id, node] : nodes) {
        if (id == node_id) continue; // 跳過新加的 node

        vector<pair<string,string>> to_move;
        for (auto& kv : node->get_all()) {
            string target = ring.get_node(kv.first);
            if (target == node_id) {
                to_move.push_back(kv);
            }
        }

        for (auto& kv : to_move) {
            node->del(kv.first);
            nodes[node_id]->put(kv.first, kv.second);
        }
    }
}

void Coordinator::remove_node(const string& node_id) {
    if (!nodes.count(node_id)) {
        cout << "Node not found: " << node_id << "\n";
        return;
    }

    // 先拿出該 node 的所有資料
    vector<pair<string,string>> backup = nodes[node_id]->get_all();

    nodes.erase(node_id);
    ring.remove_node(node_id);

    cout << "Rebalancing after removing node " << node_id << "...\n";

    // 把資料重新放回其他節點
    for (auto& kv : backup) {
        put(kv.first, kv.second);
    }
}

void Coordinator::put(const string& key, const string& value) {
    string node_id = ring.get_node(key);
    if (node_id.empty()) {
        cout << "No available nodes\n";
        return;
    }
    nodes[node_id]->put(key, value);
}

string Coordinator::get(const string& key) {
    string node_id = ring.get_node(key);
    if (node_id.empty()) {
        return "No available nodes";
    }
    return nodes[node_id]->get(key);
}

void Coordinator::del(const string& key) {
    string node_id = ring.get_node(key);
    if (node_id.empty()) {
        cout << "No available nodes\n";
        return;
    }
    nodes[node_id]->del(key);
}

void Coordinator::show_nodes() {
    cout << "Active Nodes:\n";
    for (auto& [id, node] : nodes) {
        cout << "  " << id << " (size=" << node->get_size() << ")\n";
    }
}

void Coordinator::show_ring() {
    ring.show_ring();
}
