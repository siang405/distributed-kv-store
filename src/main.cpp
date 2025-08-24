#include <iostream>
#include <sstream>
#include "coordinator.hpp"

int main() {
    Coordinator coord;

    std::cout << "Distributed KV Store with Consistent Hashing\n";
    std::cout << "Commands: addnode <id>, removenode <id>, put <k> <v>, get <k>, del <k>, show\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        std::istringstream iss(line);

        std::string cmd;
        iss >> cmd;
        if (cmd == "addnode") {
            std::string id; iss >> id;
            coord.add_node(id);
        } else if (cmd == "removenode") {
            std::string id; iss >> id;
            coord.remove_node(id);
        } else if (cmd == "put") {
            std::string k, v; iss >> k >> v;
            coord.put(k, v);
        } else if (cmd == "get") {
            std::string k; iss >> k;
            std::cout << coord.get(k) << "\n";
        } else if (cmd == "del") {
            std::string k; iss >> k;
            coord.del(k);
        } else if (cmd == "show") {
            coord.show_nodes();
        } else if (cmd == "exit") {
            break;
        }
    }

    return 0;
}
