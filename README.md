# Distributed KV Store with Consistent Hashing

This project implements a simple **distributed key-value store simulator** in C++ using **consistent hashing**.
It demonstrates how distributed systems manage **node addition/removal** with **minimal data movement**.

## Features

* **Consistent Hashing**

  * Keys are mapped to nodes via a hash ring.
  * Supports virtual nodes (replicas) to balance load.
* **Node Management**

  * Add or remove nodes dynamically.
  * Automatic rebalancing of keys across nodes.
* **Key-Value Operations**

  * `put <key> <value>`: insert or update a key.
  * `get <key>`: retrieve a value.
  * `del <key>`: delete a key.
* **CLI Commands**

  * `addnode <id>`: add a node to the ring.
  * `removenode <id>`: remove a node and redistribute its data.
  * `show`: list all active nodes and their data sizes.
* **Rebalancing**

  * When a node is added or removed, only affected keys are moved.
  * Ensures minimal disruption, a core property of consistent hashing.

## Build & Run

```bash
mkdir build && cd build
cmake ..
make
./runtime
```

## Example CLI Session

```
Distributed KV Store with Consistent Hashing
Commands: addnode <id>, removenode <id>, put <k> <v>, get <k>, del <k>, show

> addnode A
Node added: A (with 3 replicas)
Rebalancing after adding node A...

> addnode B
Node added: B (with 3 replicas)
Rebalancing after adding node B...

> put apple 1
[B] PUT apple = 1

> put banana 2
[B] PUT banana = 2

> put cherry 3
[B] PUT cherry = 3

> show
Active Nodes:
  B (size=3)
  A (size=0)

> addnode C
Node added: C (with 3 replicas)
Rebalancing after adding node C...
[C] PUT apple = 1
[C] PUT banana = 2
[C] PUT cherry = 3

> show
Active Nodes:
  B (size=0)
  C (size=3)
  A (size=0)
```

## Future Work

* Persistence layer (store data on disk instead of memory).
* Replication factor for fault tolerance.
* Integration with network layer for real distributed testing.
* Benchmarking tools for key distribution and load balance analysis.

---

MIT License © 2025
