#include <iostream>
#include <unordered_map>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
int op_count = 0;           
const int COMPACT_THRESHOLD = 100; 
unordered_map<string, string> store;
const string DATA_FILE = "data.log";

void compact() {
    ofstream ofs(DATA_FILE, ios::trunc);
    for (auto& [k, v] : store) {
        ofs << "PUT " << k << " " << v << "\n";
    }
    cout << "[Compaction] data.log compacted, size=" << store.size() << endl;
}

void persist_append(const string& op, const string& key, const string& value = "") {
    ofstream ofs(DATA_FILE, ios::app);
    if (op == "PUT") {
        ofs << "PUT " << key << " " << value << "\n";
    } else if (op == "DEL") {
        ofs << "DEL " << key << "\n";
    }

    if (++op_count >= COMPACT_THRESHOLD) {
        compact();
        op_count = 0;
    }
}

void load_data() {
    ifstream ifs(DATA_FILE);
    string op, key, value;
    while (ifs >> op) {
        if (op == "PUT") {
            ifs >> key >> value;
            store[key] = value;
        } else if (op == "DEL") {
            ifs >> key;
            store.erase(key);
        }
    }
}

string handle_request(const string& req) {
    json j = json::parse(req);
    string op = j["op"];

    if (op == "put") {
        string key = j["key"];
        string val = j["value"];
        store[key] = val;
        persist_append("PUT", key, val);
        return json({{"status", "ok"}}).dump();

    } else if (op == "get") {
        string key = j["key"];
        if (store.count(key))
            return json({{"status", "ok"}, {"value", store[key]}}).dump();
        else
            return json({{"status", "not_found"}}).dump();

    } else if (op == "del") {
        string key = j["key"];
        if (store.erase(key)) {
            persist_append("DEL", key);
            return json({{"status", "ok"}}).dump();
        } else {
            return json({{"status", "not_found"}}).dump();
        }

    } else if (op == "size") {
        return json({{"status", "ok"}, {"keys", (int)store.size()}}).dump();

    } else if (op == "dump") {
        json data;
        for (auto& [k, v] : store) {
            data[k] = v;
        }
        return json({{"status","ok"}, {"data", data}}).dump();
    }

    return json({{"status", "error"}, {"msg", "unknown op"}}).dump();
}

int main(int argc, char* argv[]) {
    int port = 5000;
    if (argc > 1) port = stoi(argv[1]);

    load_data();

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    cout << "Node server listening on port " << port << endl;

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);

        string response = handle_request(string(buffer, valread));
        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }
}
