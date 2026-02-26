#pragma once
#include <iostream>
#include <sqlite3.h>
#include <string>

#include "json.hpp"

using json = nlohmann::json;

class DatabaseManager
{
private:
    sqlite3 *db;

public:
    DatabaseManager(const std::string &db_path);
    ~DatabaseManager();

    bool create_tables();
    bool add_user(const json &user_data);
    json authenticate_user(const std::string &email, const std::string &password);
};