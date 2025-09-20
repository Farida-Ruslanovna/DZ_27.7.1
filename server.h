#ifndef SERVER_H
#define SERVER_H

#include "network.h"
#include "db_manager.h"
#include "logger.h"
#include <string>
#include <vector>

class Server {
private:
    int listenSocket;
    DBManager db;
    Logger logger;

public:
    Server();
    ~Server();
    bool start(int port);
    void run();
    void handleClient(int clientSocket);
};

#endif