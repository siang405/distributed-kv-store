#include <iostream>
#include <sstream>
#include "coordinator.hpp"

using namespace std;

int main() {
    Coordinator coord;

    cout << "Distributed KV Store with Consistent Hashing (Networked)\n";
    cout << "Commands: addnode <id> <port>, removenode <id>, put <k> <v>, get <k>, del <k>, show, exit\n";

    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        istringstream iss(line);

        string cmd;
        iss >> cmd;
        if (cmd == "addnode") {
            string id; int port;
            iss >> id >> port;
            coord.add_node(id, port);
        } else if (cmd == "removenode") {
            string id; iss >> id;
            coord.remove_node(id);
        } else if (cmd == "put") {
            string k, v; iss >> k >> v;
            coord.put(k, v);
        } else if (cmd == "get") {
            string k; iss >> k;
            string value = coord.get(k);
            cout << k << " -> " << value << "\n";
        } else if (cmd == "del") {
            string k; iss >> k;
            coord.del(k);
        } else if (cmd == "show") {
            coord.show_nodes();
        } else if (cmd == "exit") {
            break;
        }else if (cmd == "stats") {
            coord.show_stats();
        }else if (cmd == "setreplica") {
            int n;
            iss >> n;
            coord.set_replica(n);
        }

    }
    return 0;
}
