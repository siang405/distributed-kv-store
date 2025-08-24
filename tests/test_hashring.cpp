#include "hash_ring.hpp"
#include <iostream>
using namespace std;

int main() {
    HashRing ring(3);

    ring.add_node("NodeA");
    ring.add_node("NodeB");
    ring.add_node("NodeC");

    ring.show_ring();

    cout << "Key=apple -> " << ring.get_node("apple") << "\n";
    cout << "Key=banana -> " << ring.get_node("banana") << "\n";
    cout << "Key=cherry -> " << ring.get_node("cherry") << "\n";

    ring.remove_node("NodeB");
    ring.show_ring();

    return 0;
}
