#include "hash_ring.hpp"
#include <iostream>
#include <functional>
using namespace std;

// 建構子
HashRing::HashRing(int replicas) : replicas(replicas) {}

// 雜湊函式
size_t HashRing::hash_fn(const string& key) {
    return hash<string>{}(key);
}

// 加入節點
void HashRing::add_node(const string& node_id) {
    for (int i = 0; i < replicas; i++) {
        string vnode = node_id + "#" + to_string(i);
        size_t h = hash_fn(vnode);
        ring[h] = node_id;
    }
    cout << "Node added: " << node_id << " (with " << replicas << " replicas)\n";
}

// 移除節點
void HashRing::remove_node(const string& node_id) {
    for (int i = 0; i < replicas; i++) {
        string vnode = node_id + "#" + to_string(i);
        size_t h = hash_fn(vnode);
        ring.erase(h);
    }
    cout << "Node removed: " << node_id << "\n";
}

// 找到 key 所屬節點
string HashRing::get_node(const string& key) {
    if (ring.empty()) return "";

    size_t h = hash_fn(key);

    auto it = ring.lower_bound(h);
    if (it == ring.end()) {
        return ring.begin()->second;
    }
    return it->second;
}

// 顯示 ring 狀態
void HashRing::show_ring() {
    cout << "Hash Ring:\n";
    for (auto& [h, node] : ring) {
        cout << "  " << h << " -> " << node << "\n";
    }
}
