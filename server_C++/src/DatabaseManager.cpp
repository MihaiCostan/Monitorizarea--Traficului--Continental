#include "DatabaseManager.hpp"

DatabaseManager::DatabaseManager(const std::string &db_path)
{
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Nu s-a putut deschide baza de date!\n";
    }
    else
    {
        create_tables();
    }
}

DatabaseManager::~DatabaseManager() { sqlite3_close(db); }

bool DatabaseManager::create_tables()
{
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "email TEXT PRIMARY KEY, "
                      "password TEXT, "
                      "nume TEXT, "
                      "masina TEXT, "
                      "numar_masina TEXT, "
                      "user_id TEXT);";
    char *err_msg = 0;
    return sqlite3_exec(db, sql, 0, 0, &err_msg) == SQLITE_OK;
}

bool DatabaseManager::add_user(const json &j)
{
    const char *sql = "INSERT INTO users VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    // Legăm datele de semnele de întrebare (?)
    sqlite3_bind_text(stmt, 1, j["email"].get<std::string>().c_str(), -1,
                      SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, j["password"].get<std::string>().c_str(), -1,
                      SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, j["nume"].get<std::string>().c_str(), -1,
                      SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, j["masina"].get<std::string>().c_str(), -1,
                      SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, j["numar_masina"].get<std::string>().c_str(), -1,
                      SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, j["user_id"].get<std::string>().c_str(), -1,
                      SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

json DatabaseManager::authenticate_user(const std::string &email,
                                        const std::string &password)
{
    const char *sql = "SELECT * FROM users WHERE email = ? AND password = ?;";
    sqlite3_stmt *stmt;
    json result = {{"status", "fail"}};

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            result["status"] = "success";
            result["nume"] = (const char *)sqlite3_column_text(stmt, 2);
            result["masina"] = (const char *)sqlite3_column_text(stmt, 3);
            result["numar_masina"] = (const char *)sqlite3_column_text(stmt, 4);
            result["user_id"] = (const char *)sqlite3_column_text(stmt, 5);
        }
    }
    sqlite3_finalize(stmt);
    return result;
}