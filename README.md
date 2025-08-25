# Distributed KV Store with Consistent Hashing

A simple **distributed key-value store** built in C++ that demonstrates:

* **Consistent Hashing** for load balancing
* **Node join/leave** with automatic rebalancing
* **TCP-based communication** between coordinator and storage nodes
* **CLI interface** for interactive operations
* **Statistics & monitoring** (`stats` command)

This project is designed to simulate core ideas behind distributed databases and caching systems (like DynamoDB, Cassandra, or Redis Cluster).

---

## 🚀 Features

* **Consistent Hashing Ring**

  * Supports virtual nodes (replicas) to smooth load distribution
* **Dynamic Node Management**

  * `addnode <id> <port>` and `removenode <id>` with automatic data rebalancing
* **Core KV Operations**

  * `put <k> <v>`, `get <k>`, `del <k>`
* **Monitoring**

  * `show` → Displays all active nodes with their key counts
  * `stats` → Displays total keys, number of nodes, and average keys per node
* **JSON-based RPC**

  * Lightweight messaging using [nlohmann/json](https://github.com/nlohmann/json)

---

## 🛠 Build Instructions

### Requirements

* C++17 or higher
* CMake >= 3.10
* POSIX sockets (Linux/macOS)

### Build

```bash
mkdir build && cd build
cmake ..
make
```

---

## ▶️ Run the System

Start multiple node servers (in separate terminals):

```bash
./node_server 5001
./node_server 5002
```

Run the coordinator CLI:

```bash
./runtime
```

---

## 💻 CLI Commands

| Command               | Description                                                   |
| --------------------- | ------------------------------------------------------------- |
| `addnode <id> <port>` | Add a new storage node at given port                          |
| `removenode <id>`     | Remove a storage node                                         |
| `put <key> <value>`   | Store key-value pair                                          |
| `get <key>`           | Retrieve value for a key                                      |
| `del <key>`           | Delete a key                                                  |
| `show`                | Show active nodes with their key counts                       |
| `stats`               | Show system-wide stats (nodes, total keys, avg keys per node) |
| `exit`                | Exit the coordinator CLI                                      |

---

## 📝 Example Session

```
Distributed KV Store with Consistent Hashing
Commands: addnode <id> <port>, removenode <id>, put <k> <v>, get <k>, del <k>, show, stats, exit

> addnode A 5001
Node added: A (127.0.0.1:5001)
Rebalancing after adding node A...

> addnode B 5002
Node added: B (127.0.0.1:5002)
Rebalancing after adding node B...

> put apple 123
[A] PUT apple = 123 -> {"status":"ok"}

> put banana 456
[B] PUT banana = 456 -> {"status":"ok"}

> get apple
apple -> 123

> show
Active Nodes:
  A (127.0.0.1:5001, keys=1)
  B (127.0.0.1:5002, keys=1)

> stats

[System Stats]
Nodes: 2
Total Keys: 2
Avg Keys per Node: 1
```

---

## 📌 Future Work

* Replication factor > 1 (data redundancy)
* Fault tolerance (node crash recovery)
* Smarter rebalance policies
* Benchmarking & visualization

---

MIT License © 2025
