#ifndef USER_H
#define USER_H

#include <string>
#include <fstream>
#include <sys/stat.h> // для chmod
#include <unistd.h>   // для access

// Класс User для хранения данных пользователя
class User {
private:
    std::string _name;  // Имя пользователя
    std::string _login; // Логин
    std::string _pass;  // Пароль

public:
    User() = default;

    // Конструктор с инициализацией
    User(const std::string& name, const std::string& login, const std::string& pass)
        : _name(name), _login(login), _pass(pass) {
    }

    // Геттеры
    std::string getName() const { return _name; }
    std::string getLogin() const { return _login; }
    std::string getPass() const { return _pass; }

    // Сеттеры
    void setName(const std::string& name) { _name = name; }
    void setLogin(const std::string& login) { _login = login; }
    void setPass(const std::string& pass) { _pass = pass; }

    // Проверка существования файла
    static bool fileExists(const std::string& filename) {
        return access(filename.c_str(), F_OK) == 0;
    }

    // Загрузка данных из файла
    bool loadFromFile(const std::string& filename);
    // Сохранение данных в файл
    bool saveToFile(const std::string& filename) const;
};

#endif

