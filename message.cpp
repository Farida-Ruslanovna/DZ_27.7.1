#include "message.h"

// Сохранение сообщения
bool Message::saveToFile(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::trunc);
    if (!ofs)
        return false;

    ofs << _text << '\n' << _sender << '\n' << _receiver << '\n';

    if (chmod(filename.c_str(), S_IRUSR | S_IWUSR) != 0)
        return false;

    return true;
}

// Загрузка сообщения
bool Message::loadFromFile(const std::string& filename) {
    if (access(filename.c_str(), F_OK) != 0)
        return false;

    std::ifstream ifs(filename);
    if (!ifs)
        return false;

    if (!std::getline(ifs, _text)) return false;
    if (!std::getline(ifs, _sender)) return false;
    if (!std::getline(ifs, _receiver)) return false;

    return true;
}
