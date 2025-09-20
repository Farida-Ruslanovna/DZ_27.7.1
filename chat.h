#ifndef CHAT_H
#define CHAT_H

#include <vector>
#include <string>
#include "message.h"
#include <fstream>
#include <sys/stat.h> // для chmod
#include <unistd.h>   // для access

// Класс Chat для управления коллекцией сообщений
class Chat {
private:
    std::vector<Message> messages; // Вектор сообщений

public:
    // Добавить сообщение
    void addMessage(const Message& msg);
    // Вывести все сообщения
    void showMessages() const;
    // Сохранить все сообщения в файл
    void saveToFile(const std::string& filename) const;
    // Загрузить сообщения из файла
    void loadFromFile(const std::string& filename);
};

#endif