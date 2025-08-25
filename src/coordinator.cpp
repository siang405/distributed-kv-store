#include "coordinator.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

Coordinator::Coordinator() : ring(3) {}

string Coordinator::send_request(const string& host, int port, const json& j) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) throw runtime_error("Socket creation failed");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0)
        throw runtime_error("Invalid address");

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        throw runtime_error("Connection failed");

    string msg = j.dump();
    send(sock, msg.c_str(), msg.size(), 0);

    char buffer[4096] = {0};
    int len = read(sock, buffer, sizeof(buffer));
    close(sock);

    return string(buffer, len);
}

void Coordinator::add_node(const string& id, int port) {
    ring.add_node(id);
    nodes[id] = {"127.0.0.1", port};
    cout << "Node added: " << id << " (127.0.0.1:" << port << ")\n";
    cout << "Rebalancing after adding node " << id << "...\n";
}

void Coordinator::remove_node(const string& id) {
    ring.remove_node(id);
    nodes.erase(id);
    cout << "Node removed: " << id << "\n";
}

void Coordinator::put(const string& key, const string& value) {
    string node_id = ring.get_node(key);
    if (nodes.count(node_id)) {
        auto n = nodes[node_id];
        json req = {{"op","put"}, {"key",key}, {"value",value}};
        auto resp = send_request(n.host, n.port, req);
        cout << "[" << node_id << "] PUT " << key << " = " << value
             << " -> " << resp << "\n";
    }
}

string Coordinator::get(const string& key) {
    string node_id = ring.get_node(key);
    if (nodes.count(node_id)) {
        auto n = nodes[node_id];
        json req = {{"op","get"}, {"key",key}};
        auto resp_str = send_request(n.host, n.port, req);

        try {
            auto resp = json::parse(resp_str);
            if (resp.contains("status") && resp["status"] == "ok" && resp.contains("value")) {
                return resp["value"];
            } else {
                return "[not found]";
            }
        } catch (...) {
            return "[error parsing response]";
        }
    }
    return "[no node]";
}

void Coordinator::del(const string& key) {
    string node_id = ring.get_node(key);
    if (nodes.count(node_id)) {
        auto n = nodes[node_id];
        json req = {{"op","del"}, {"key",key}};
        auto resp = send_request(n.host, n.port, req);
        cout << "[" << node_id << "] DEL " << key << " -> " << resp << "\n";
    }
}

void Coordinator::show_nodes() const {
    cout << "Active Nodes:\n";
    for (const auto& [id, n] : nodes) {
        json req = {{"op","size"}};
        auto resp_str = send_request(n.host, n.port, req);
        int key_count = -1;
        try {
            auto resp = json::parse(resp_str);
            if (resp.contains("status") && resp["status"] == "ok") {
                key_count = resp.value("keys", -1);
            }
        } catch (...) {
            // ignore parse errors
        }
        cout << "  " << id << " (" << n.host << ":" << n.port
             << ", keys=" << key_count << ")\n";
    }
}

void Coordinator::show_stats() const {
    int total_keys = 0;
    int node_count = nodes.size();

    cout << "\n[System Stats]\n";
    for (const auto& [id, n] : nodes) {
        json req = {{"op","size"}};
        auto resp_str = send_request(n.host, n.port, req);
        int key_count = 0;
        try {
            auto resp = json::parse(resp_str);
            if (resp.contains("status") && resp["status"] == "ok") {
                key_count = resp.value("keys", 0);
            }
        } catch (...) {
            key_count = -1; // failed request
        }
        total_keys += max(0, key_count);
    }

    cout << "Nodes: " << node_count << "\n";
    cout << "Total Keys: " << total_keys << "\n";
    if (node_count > 0) {
        cout << "Avg Keys per Node: " << (total_keys / node_count) << "\n";
    }
    cout << endl;
}
