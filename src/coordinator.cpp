#include "coordinator.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std;
using json = nlohmann::json;

Coordinator::Coordinator() : ring(3) {
    start_heartbeat();
}

Coordinator::~Coordinator() {
    stop();
}

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
    node_status[id] = true;
    cout << "Node added: " << id << " (127.0.0.1:" << port << ")\n";
    cout << "Rebalancing after adding node " << id << "...\n";
}

void Coordinator::remove_node(const string& id) {
    ring.remove_node(id);
    nodes.erase(id);
    node_status.erase(id);
    cout << "Node removed: " << id << "\n";
}

void Coordinator::put(const string& key, const string& value) {
    auto node_ids = ring.get_nodes(key, replicaN);
    for (auto& node_id : node_ids) {
        if (nodes.count(node_id)) {
            auto n = nodes[node_id];
            json req = {{"op","put"}, {"key",key}, {"value",value}};
            try {
                auto resp = send_request(n.host, n.port, req);
                cout << "[" << node_id << "] PUT " << key << " = " << value
                     << " -> " << resp << "\n";
            } catch (...) {
                cerr << "[WARN] PUT failed on " << node_id << endl;
                node_status[node_id] = false;
            }
        }
    }
}

string Coordinator::get(const string& key) {
    auto node_ids = ring.get_nodes(key, replicaN);
    for (auto& node_id : node_ids) {
        if (nodes.count(node_id)) {
            auto n = nodes[node_id];
            json req = {{"op","get"}, {"key",key}};
            try {
                auto resp_str = send_request(n.host, n.port, req);
                auto resp = json::parse(resp_str);
                if (resp.contains("status") && resp["status"] == "ok" && resp.contains("value")) {
                    return resp["value"];
                }
            } catch (...) {
                cerr << "[WARN] GET failed on " << node_id << endl;
                node_status[node_id] = false;
            }
        }
    }
    return "[not found]";
}

void Coordinator::del(const string& key) {
    auto node_ids = ring.get_nodes(key, replicaN);
    for (auto& node_id : node_ids) {
        if (nodes.count(node_id)) {
            auto n = nodes[node_id];
            json req = {{"op","del"}, {"key",key}};
            try {
                auto resp = send_request(n.host, n.port, req);
                cout << "[" << node_id << "] DEL " << key << " -> " << resp << "\n";
            } catch (...) {
                cerr << "[WARN] DEL failed on " << node_id << endl;
                node_status[node_id] = false;
            }
        }
    }
}

void Coordinator::show_nodes() const {
    cout << "Active Nodes:\n";
    for (const auto& [id, n] : nodes) {
        json req = {{"op","size"}};
        int key_count = -1;
        string status_str = "DOWN";
        try {
            auto resp_str = send_request(n.host, n.port, req);
            auto resp = json::parse(resp_str);
            if (resp.contains("status") && resp["status"] == "ok") {
                key_count = resp.value("keys", -1);
                status_str = "UP";
            }
        } catch (...) {
            // ignore errors
        }
        cout << "  " << id << " (" << n.host << ":" << n.port
             << ", keys=" << key_count
             << ", status=" << status_str << ")\n";
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
            key_count = -1;
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

void Coordinator::start_heartbeat() {
    thread([this]() {
        while (running) {
            for (auto& [id, n] : nodes) {
                json req = {{"op", "size"}};
                try {
                    auto resp = send_request(n.host, n.port, req);
                    auto j = json::parse(resp);
                    if (j.contains("status") && j["status"] == "ok") {
                        node_status[id] = true;
                    } else {
                        node_status[id] = false;
                    }
                } catch (...) {
                    node_status[id] = false;
                }
            }
            this_thread::sleep_for(chrono::seconds(3));
        }
    }).detach();
}

void Coordinator::stop() {
    running = false;
}

void Coordinator::rebalance() {
    cout << "[Coordinator] Starting rebalance...\n";

    for (auto& [id, n] : nodes) {
        json req = {{"op","dump"}};
        try {
            auto resp_str = send_request(n.host, n.port, req);
            auto resp = json::parse(resp_str);

            if (resp.contains("status") && resp["status"] == "ok") {
                for (auto& [k, v] : resp["data"].items()) {
                    auto correct_nodes = ring.get_nodes(k, replicaN);

                    for (auto& target_id : correct_nodes) {
                        if (nodes.count(target_id)) {
                            auto target = nodes[target_id];
                            json put_req = {{"op","put"}, {"key",k}, {"value",v}};
                            send_request(target.host, target.port, put_req);

                            if (target_id != id) {
                                cout << "[Rebalance] Copied key '" << k
                                     << "' to " << target_id << endl;
                            }
                        }
                    }

                    if (find(correct_nodes.begin(), correct_nodes.end(), id) == correct_nodes.end()) {
                        json del_req = {{"op","del"}, {"key",k}};
                        send_request(n.host, n.port, del_req);
                        cout << "[Rebalance] Removed key '" << k
                             << "' from " << id << endl;
                    }
                }
            }
        } catch (...) {
            cerr << "[WARN] rebalance failed on " << id << endl;
        }
    }

    cout << "[Coordinator] Rebalance complete.\n";
}
