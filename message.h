#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <fstream>
#include <sys/stat.h> // для chmod
#include <unistd.h>   // для access

// Класс Message для хранения сообщений
class Message {
private:
    std::string _text;     // Текст сообщения
    std::string _sender;   // Отправитель
    std::string _receiver; // Получатель

public:
    Message() = default;
    Message(const std::string& text, const std::string& sender, const std::string& receiver)
        : _text(text), _sender(sender), _receiver(receiver) {
    }

    // Геттеры
    std::string getText() const { return _text; }
    std::string getSender() const { return _sender; }
    std::string getReceiver() const { return _receiver; }

    // Сеттеры
    void setText(const std::string& text) { _text = text; }
    void setSender(const std::string& sender) { _sender = sender; }
    void setReceiver(const std::string& receiver) { _receiver = receiver; }

    // Сохранить сообщение в файл
    bool saveToFile(const std::string& filename) const;
    // Загрузить сообщение из файла
    bool loadFromFile(const std::string& filename);
};

#endif