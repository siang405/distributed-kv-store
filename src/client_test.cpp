// src/client_test.cpp
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

string send_request(const string& msg, int port = 5000) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    send(sock, msg.c_str(), msg.size(), 0);
    int valread = read(sock, buffer, 1024);
    close(sock);
    return string(buffer, valread);
}

int main() {
    cout << send_request(json({{"op","put"}, {"key","apple"}, {"value","123"}}).dump()) << endl;
    cout << send_request(json({{"op","get"}, {"key","apple"}}).dump()) << endl;
    cout << send_request(json({{"op","del"}, {"key","apple"}}).dump()) << endl;
    cout << send_request(json({{"op","get"}, {"key","apple"}}).dump()) << endl;
    return 0;
}
