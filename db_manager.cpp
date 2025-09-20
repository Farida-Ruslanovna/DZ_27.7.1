#include "db_manager.h"
#include <iostream>

DBManager::DBManager() : env(SQL_NULL_HENV), dbc(SQL_NULL_HDBC) {
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate environment handle.\n";
        env = SQL_NULL_HENV;
        return;
    }
    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to set environment attributes.\n";
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        env = SQL_NULL_HENV;
        return;
    }
}

// Деструктор освобождает ресурсы
DBManager::~DBManager() {
    disconnect();
    if (env != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, env);
    }
}

// Подключение к базе данных
bool DBManager::connect(const std::string& dsn, const std::string& user, const std::string& password) {
    if (env == SQL_NULL_HENV) return false;

    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate connection handle.\n";
        return false;
    }

    // Формируем строку соединения
    std::string connStr = "DSN=" + dsn + ";UID=" + user + ";PWD=" + password + ";";

    // Устанавливаем соединение
    ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*)connStr.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to connect to database.\n";
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        dbc = SQL_NULL_HDBC;
        return false;
    }
    return true;
}

// Отключение от базы данных
void DBManager::disconnect() {
    if (dbc != SQL_NULL_HDBC) {
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        dbc = SQL_NULL_HDBC;
    }
}

// Регистрация нового пользователя
bool DBManager::registerUser(const std::string& login, const std::string& pass, const std::string& name) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    const char* query = "INSERT INTO users (login, password, name) VALUES (?, ?, ?)";
    if (SQLPrepare(stmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false; // Ошибка подготовки запроса
    }

    // Связываем параметры
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, login.size(), 0, (SQLPOINTER)login.c_str(), 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, pass.size(), 0, (SQLPOINTER)pass.c_str(), 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, name.size(), 0, (SQLPOINTER)name.c_str(), 0, NULL);

    // Выполняем запрос
    SQLRETURN ret = SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

// Вход пользователя — возвращает ID или -1, если не найден
int DBManager::loginUser(const std::string& login, const std::string& pass) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    const char* query = "SELECT id FROM users WHERE login=? AND password=?";
    if (SQLPrepare(stmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return -1; // Ошибка подготовки
    }

    // Связываем параметры
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, login.size(), 0, (SQLPOINTER)login.c_str(), 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, pass.size(), 0, (SQLPOINTER)pass.c_str(), 0, NULL);

    // Выполняем запрос
    if (SQLExecute(stmt) != SQL_SUCCESS && SQLExecute(stmt) != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return -1; // Не удалось выполнить или нет совпадений
    }

    SQLINTEGER userId;
    SQLLEN indicator;

    // Связываем колонку с переменной
    if (SQLBindCol(stmt, 1, SQL_C_SLONG, &userId, 0, &indicator) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return -1;
    }

    // Получаем результат
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return (int)userId; // Возвращаем ID пользователя
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return -1; // Пользователь не найден
}

// Получение списка всех пользователей
std::vector<std::pair<int, std::string>> DBManager::getUsers() {
    std::vector<std::pair<int, std::string>> users;
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    const char* query = "SELECT id, name FROM users";

    if (SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return users; // Пустой список при ошибке
    }

    SQLINTEGER id;
    SQLCHAR name[256];
    SQLLEN idInd, nameInd;

    SQLBindCol(stmt, 1, SQL_C_SLONG, &id, 0, &idInd);
    SQLBindCol(stmt, 2, SQL_C_CHAR, &name, sizeof(name), &nameInd);

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        users.emplace_back((int)id, std::string((char*)name));
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return users;
}

// Получение истории сообщений между двумя пользователями
std::vector<std::string> DBManager::getMessages(int userId1, int userId2) {
    std::vector<std::string> msgs;
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Запрос для получения сообщений в обе стороны между двумя пользователями
    const char* query = "SELECT u1.name, u2.name, m.text, m.timestamp "
        "FROM messages m "
        "JOIN users u1 ON m.sender_id = u1.id "
        "JOIN users u2 ON m.receiver_id = u2.id "
        "WHERE (m.sender_id=? AND m.receiver_id=?) "
        "OR (m.sender_id=? AND m.receiver_id=?) "
        "ORDER BY m.timestamp";

    if (SQLPrepare(stmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return msgs;
    }

    // Связываем параметры
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &userId1, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &userId2, 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &userId2, 0, NULL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &userId1, 0, NULL);

    if (SQLExecute(stmt) != SQL_SUCCESS && SQLExecute(stmt) != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return msgs;
    }

    // Колонки результата
    SQLCHAR sender[256], receiver[256], text[1024], timestamp[256];
    SQLLEN senderInd, receiverInd, textInd, timestampInd;

    SQLBindCol(stmt, 1, SQL_C_CHAR, sender, sizeof(sender), &senderInd);
    SQLBindCol(stmt, 2, SQL_C_CHAR, receiver, sizeof(receiver), &receiverInd);
    SQLBindCol(stmt, 3, SQL_C_CHAR, text, sizeof(text), &textInd);
    SQLBindCol(stmt, 4, SQL_C_CHAR, timestamp, sizeof(timestamp), &timestampInd);

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        std::string msgStr = std::string((char*)sender) + " -> " + std::string((char*)receiver) + ": " +
            std::string((char*)text) + " [" + std::string((char*)timestamp) + "]";
        msgs.push_back(msgStr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return msgs;
}

// Отправка сообщения от одного пользователя другому
bool DBManager::sendMessage(int senderId, int receiverId, const std::string& text) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    const char* query = "INSERT INTO messages (sender_id, receiver_id, text, timestamp) VALUES (?, ?, ?, CURRENT_TIMESTAMP)";
    if (SQLPrepare(stmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    // Связываем параметры
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &senderId, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &receiverId, 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, text.size(), 0, (SQLPOINTER)text.c_str(), 0, NULL);

    // Выполняем
    SQLRETURN ret = SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}