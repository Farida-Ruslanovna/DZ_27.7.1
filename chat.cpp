#include "chat.h"
#include <iostream>

// Добавление сообщения
void Chat::addMessage(const Message& msg) {
    messages.push_back(msg);
}

// Вывод всех сообщений
void Chat::showMessages() const {
    for (const auto& msg : messages) {
        std::cout << "От: " << msg.getSender()
            << " Кому: " << msg.getReceiver()
            << "\nСообщение: " << msg.getText() << "\n\n";
    }
}

// Сохранение в файл
void Chat::saveToFile(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::trunc);
    if (!ofs)
        return;

    for (const auto& msg : messages) {
        ofs << msg.getText() << '\n' << msg.getSender() << '\n' << msg.getReceiver() << '\n';
    }

    if (chmod(filename.c_str(), S_IRUSR | S_IWUSR) != 0) {
        // Можно логировать ошибку
    }
}

// Загрузка из файла
void Chat::loadFromFile(const std::string& filename) {
    messages.clear();

    std::ifstream ifs(filename);
    if (!ifs)
        return;

    while (true) {
        std::string text, sender, receiver;
        if (!std::getline(ifs, text))
            break;
        if (text.empty()) // можно пропускать пустые
            break;
        if (!std::getline(ifs, sender)) break;
        if (!std::getline(ifs, receiver)) break;

        messages.emplace_back(text, sender, receiver);
    }
}