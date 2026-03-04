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
    bool add_accident(double lat, double lng, const std::string &strada, const std::string &detalii);
    json authenticate_user(const std::string &email, const std::string &password);
    json get_speed_limit_at_location(double lat, double lng);
    json get_all_accidents();
};