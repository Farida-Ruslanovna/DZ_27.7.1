#include "network.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#endif

bool Network::initialize() {
#ifdef _WIN32
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        std::cerr << "WSAStartup failed: " << res << "\n";
        return false;
    }
#endif
    return true;
}

void Network::cleanup() {
#ifdef _WIN32
    WSACleanup();
#endif
}