// src/node_server.cpp
#include <iostream>
#include <unordered_map>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <nlohmann/json.hpp>  // JSON library (你可以在 CMakeLists.txt 加上)

using namespace std;
using json = nlohmann::json;

unordered_map<string, string> store;

string handle_request(const string& req) {
    json j = json::parse(req);
    string op = j["op"];

    if (op == "put") {
        store[j["key"]] = j["value"];
        return json({{"status", "ok"}}).dump();
    } else if (op == "get") {
        string key = j["key"];
        if (store.count(key))
            return json({{"status", "ok"}, {"value", store[key]}}).dump();
        else
            return json({{"status", "not_found"}}).dump();
    } else if (op == "del") {
        string key = j["key"];
        if (store.erase(key))
            return json({{"status", "ok"}}).dump();
        else
            return json({{"status", "not_found"}}).dump();
    }else if (op == "size") {
        return json({{"status", "ok"}, {"keys", (int)store.size()}}).dump();
    }
    return json({{"status", "error"}, {"msg", "unknown op"}}).dump();
}

int main(int argc, char* argv[]) {
    int port = 5000;
    if (argc > 1) port = stoi(argv[1]);

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
