#pragma once
#include "hash_ring.hpp"
#include "node.hpp"
#include <unordered_map>
#include <memory>

class Coordinator {
public:
    Coordinator();

    void add_node(const std::string& node_id);
    void remove_node(const std::string& node_id);

    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void del(const std::string& key);

    void show_nodes();
    void show_ring();

private:
    HashRing ring;
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
};
