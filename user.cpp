#include "user.h"

// Загрузка пользователя из файла
bool User::loadFromFile(const std::string& filename) {
    if (!fileExists(filename))
        return false;

    std::ifstream ifs(filename);
    if (!ifs)
        return false;

    if (!std::getline(ifs, _name)) return false;
    if (!std::getline(ifs, _login)) return false;
    if (!std::getline(ifs, _pass)) return false;

    return true;
}

// Сохранение пользователя в файл
bool User::saveToFile(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::trunc);
    if (!ofs)
        return false;

    ofs << _name << '\n' << _login << '\n' << _pass << '\n';

    // Установка прав доступа 600
    if (chmod(filename.c_str(), S_IRUSR | S_IWUSR) != 0)
        return false;

    return true;
}
