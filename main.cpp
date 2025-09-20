// DZ_27.7.1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "network.h"
#include "server.h"
#include "client.h"
#include <string>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    if (argc < 2) {
        std::cout << "Usage:\n  " << argv[0] << " server <port>\n  " << argv[0] << " client\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server") {
        int port = 12345; // Значение по умолчанию
        if (argc >= 3) port = std::stoi(argv[2]);
        // Создаем и запускаем сервер
        Server server;
        if (server.start(port)) {
            server.run();
        }
    }
    else if (mode == "client") {
        Client client;
        client.run();
    }
    else {
        std::cerr << "Неизвестный режим.\n";
        return 1;
    }

    return 0;
}