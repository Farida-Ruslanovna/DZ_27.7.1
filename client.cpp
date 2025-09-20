#include "client.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <string>

const int PORT = 12345;

void Client::run() {
    Logger logger("log.txt");
    if (!Network::initialize()) {
        logger.log("Ошибка инициализации сети");
        return;
    }

    std::string serverIP;
    std::cout << "Введите IP сервера: ";
    std::getline(std::cin, serverIP);
    logger.log("Подключение к серверу " + serverIP);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Не удалось создать сокет\n";
        logger.log("Ошибка создания сокета");
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
#ifdef _WIN32
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
#else
    inet_aton(serverIP.c_str(), &serverAddr.sin_addr);
#endif

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка соединения\n";
        logger.log("Ошибка соединения");
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return;
    }
    logger.log("Подключено к серверу");

    // поток для получения сообщений
    std::thread recvThread([sock, &logger]() {
        char buffer[1024];
        while (true) {
            int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) {
                std::cout << "Соединение закрыто.\n";
                logger.log("Соединение закрыто");
                break;
            }
            buffer[bytesReceived] = '\0';
            std::cout << "Получено: " << buffer << "\n";
            logger.log("Получено сообщение: " + std::string(buffer));
        }
        });

    // основной поток — отправка сообщений
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") break;
        send(sock, message.c_str(), message.size(), 0);
        logger.log("Отправлено сообщение: " + message);
    }

    // закрываем соединение
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    if (recvThread.joinable()) recvThread.join();

    Network::cleanup();
    logger.log("Клиент завершил работу");
}