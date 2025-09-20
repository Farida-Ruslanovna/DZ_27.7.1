#include "logger.h"
#include <iostream>
#include <shared_mutex> // для потокобезопасности

// Конструктор: открывает файл для дозаписи
Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Не удалось открыть файл логов: " << filename << "\n";
    }
}

// Деструктор: закрывает файл
Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

// Записывает сообщение в лог (потокобезопасно)

void Logger::log(const std::string& message) const {
    std::unique_lock<std::shared_mutex> lock(mutex); // потокобезопасная эксклюзивная запись
    if (logFile.is_open()) {
        logFile << message << "\n";
        logFile.flush(); 
    }
}

// Читает одну строку из файла логов (потокобезопасно)
// Здесь используется shared_lock для чтения
std::string Logger::readLine() const {
    std::shared_lock<std::shared_mutex> lock(mutex);

    std::ifstream fin("log.txt");
    std::string line;
    if (fin.is_open() && std::getline(fin, line)) {
        return line;
    }
    return "";
}