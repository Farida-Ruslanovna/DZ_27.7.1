#include "server.h"
#include <iostream>
#include <thread>
#include <sstream>
#include <atomic>

const int PORT = 12345;

std::atomic<bool> running(true); // для остановки сервера

Server::Server() : logger("log.txt") {}

Server::~Server() {
    logger.log("Сервер завершает работу");
}

bool Server::start(int port) {
    // Инициализация сокета
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        std::cerr << "Не удалось создать сокет.\n";
        logger.log("Ошибка создания сокета");
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        std::cerr << "Ошибка привязки сокета.\n";
        logger.log("Ошибка bind");
        closesocket(listenSocket);
        return false;
    }

    if (listen(listenSocket, 5) != 0) {
        std::cerr << "Ошибка прослушивания.\n";
        logger.log("Ошибка listen");
        closesocket(listenSocket);
        return false;
    }

    // подключение к базе
    if (!db.connect("your_dsn", "user", "pass")) {
        std::cerr << "Не удалось подключиться к базе данных.\n";
        logger.log("Ошибка подключения к базе");
        closesocket(listenSocket);
        return false;
    }

    std::cout << "Сервер запущен и слушает порт " << port << "\n";
    logger.log("Сервер запущен и слушает порт " + std::to_string(port));

    // Основной цикл
    while (running) {
        int clientSock = accept(listenSocket, nullptr, nullptr);
        if (clientSock < 0) {
            std::cerr << "Ошибка accept.\n";
            logger.log("Ошибка accept");
            continue;
        }
        // создаем поток обработки клиента
        std::thread t(&Server::handleClient, this, clientSock);
        t.detach(); // оставить поток независимым
        logger.log("Обработка нового клиента запущена");
    }

    // Завершение работы
    db.disconnect();
#ifdef _WIN32
    closesocket(listenSocket);
#else
    close(listenSocket);
#endif
    return true;
}

void Server::stop() {
    running = false;
    // Можно закрыть слушающий сокет для выхода из accept
#ifdef _WIN32
    closesocket(listenSocket);
#else
    close(listenSocket);
#endif
}

void Server::handleClient(int clientSocket) {
    logger.log("Обработка клиента началась");
    char buffer[1024];
    bool clientConnected = true;

    while (clientConnected) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            clientConnected = false;
            break;
        }
        buffer[bytesReceived] = '\0';

        std::string request(buffer);
        logger.log("Получена команда: " + request);

        // Пример обработки команд
        std::istringstream iss(request);
        std::string command;
        iss >> command;

        if (command == "register") {
            // Пример: register <login> <pass> <name>
            std::string login, pass, name;
            iss >> login >> pass >> name;
            if (db.registerUser(login, pass, name)) {
                send(clientSocket, "Регистрация успешна\n", 22, 0);
                logger.log("Пользователь зарегистрирован: " + login);
            }
            else {
                send(clientSocket, "Ошибка регистрации\n", 21, 0);
                logger.log("Ошибка регистрации: " + login);
            }
        }
        else if (command == "login") {
            // Пример: login <login> <pass>
            std::string login, pass;
            int userId;
            iss >> login >> pass;
            userId = db.loginUser(login, pass);
            if (userId != -1) {
                send(clientSocket, "Успешный вход\n", 16, 0);
                logger.log("Пользователь вошел: " + login);
            }
            else {
                send(clientSocket, "Ошибка входа\n", 14, 0);
                logger.log("Ошибка входа: " + login);
            }
        }
        else if (command == "send") {
            // Пример: send <senderId> <receiverId> <message>
            int senderId, receiverId;
            std::string message;
            iss >> senderId >> receiverId;
            std::getline(iss, message);
            // Удаление начальных пробелов
            if (!message.empty() && message[0] == ' ') message.erase(0, 1);
            if (db.sendMessage(senderId, receiverId, message)) {
                send(clientSocket, "Сообщение отправлено\n", 23, 0);
                logger.log("Сообщение отправлено от " + std::to_string(senderId) + " к " + std::to_string(receiverId));
            }
            else {
                send(clientSocket, "Ошибка отправки\n", 17, 0);
                logger.log("Ошибка отправки сообщения");
            }
        }
        else if (command == "logout") {
            send(clientSocket, "Вы вышли из системы\n", 22, 0);
            clientConnected = false;
        }
        else {
            send(clientSocket, "Неизвестная команда\n", 22, 0);
        }
    }

    // закрываем соединение
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
    logger.log("Обработка клиента завершена");
}