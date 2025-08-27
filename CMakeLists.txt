include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

cmake_minimum_required(VERSION 3.10)
project(DistributedKVStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# -----------------------------
# Library: allocator core files
# -----------------------------
add_library(kvstore
    src/hash_ring.cpp
    src/node.cpp
    src/coordinator.cpp
)

target_include_directories(kvstore PUBLIC include)
target_link_libraries(kvstore PUBLIC nlohmann_json::nlohmann_json)


# -----------------------------
# Main executable (CLI)
# -----------------------------
add_executable(runtime
    src/main.cpp
    src/coordinator.cpp
    src/hash_ring.cpp
)

target_link_libraries(runtime
    PRIVATE kvstore
    PRIVATE nlohmann_json::nlohmann_json
)

# -----------------------------
# Test: HashRing demo
# -----------------------------
add_executable(test_hashring
    tests/test_hashring.cpp
)

target_link_libraries(test_hashring kvstore)

add_executable(node_server src/node_server.cpp)
target_link_libraries(node_server PRIVATE nlohmann_json::nlohmann_json)

add_executable(client_test src/client_test.cpp)
target_link_libraries(client_test PRIVATE nlohmann_json::nlohmann_json)

