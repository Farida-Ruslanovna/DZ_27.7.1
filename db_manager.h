#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include <vector>
#include <utility> // дл¤ std::pair
#include <sql.h>
#include <sqlext.h>

//  ласс дл¤ работы с базой данных через ODBC
class DBManager {
private:
    SQLHENV env;    // ќкружение ODBC
    SQLHDBC dbc;    // соединение с базой данных

public:
    DBManager();
    ~DBManager();

    bool connect(const std::string& dsn, const std::string& user, const std::string& password);
    void disconnect();

    bool registerUser(const std::string& login, const std::string& pass, const std::string& name);
    int loginUser(const std::string& login, const std::string& pass);
    std::vector<std::pair<int, std::string>> getUsers();

    std::vector<std::string> getMessages(int userId1, int userId2);
    bool sendMessage(int senderId, int receiverId, const std::string& text);
};

#endif