#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <shared_mutex> // для потокобезопасности

// Класс Logger для логирования сообщений
class Logger {
private:
    std::ofstream logFile;                 // файл логов
    mutable std::shared_mutex mutex;     // для потокобезопасных операций

public:
    // Конструктор — открывает файл логов
    explicit Logger(const std::string& filename = "log.txt");
    // Деструктор — закрывает файл
    ~Logger();

    // Запись строки в лог
    void log(const std::string& message) const;
    // Чтение одной строки из файла 
    std::string readLine() const;
};

#endif


