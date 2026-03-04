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

/// DE REPARAT DESTRUCTOR (nu sterge continutul tabelei de accidente)
DatabaseManager::~DatabaseManager()
{
    if (db)
    {
        std::string sql = "DROP TABLE IF EXISTS accidents;";
        sqlite3_exec(db, sql.c_str(), 0, 0, 0);
        std::cout << "[DB] Tabela de accidente a fost ștearsă (Curățare Sesiune).\n";

        sqlite3_close(db);
    }
}

bool DatabaseManager::create_tables()
{
    const char *users = "CREATE TABLE IF NOT EXISTS users ("
                        "email TEXT PRIMARY KEY, "
                        "password TEXT, "
                        "nume TEXT, "
                        "masina TEXT, "
                        "numar_masina TEXT, "
                        "user_id TEXT);";

    const char *cities = "CREATE TABLE IF NOT EXISTS cities ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "nume TEXT,"
                         "lat REAL, "
                         "long REAL, "
                         "raza REAL, "
                         "limita INTEGER);";

    const char *accidents = "CREATE TABLE IF NOT EXISTS accidents ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "lat REAL,"
                            "long REAL,"
                            "strada TEXT,"
                            "detalii TEXT,"
                            "limita_recomandata INTEGER DEFAULT 30," // Implicit 30 km/h
                            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    char *err_msg_users = 0;
    char *err_msg_cities = 0;
    char *err_msg_accidents = 0;
    return sqlite3_exec(db, users, 0, 0, &err_msg_users) == SQLITE_OK && sqlite3_exec(db, cities, 0, 0, &err_msg_cities) == SQLITE_OK && sqlite3_exec(db, accidents, 0, 0, &err_msg_accidents) == SQLITE_OK;
}

bool DatabaseManager::add_user(const json &j)
{
    const char *sql = "INSERT INTO users VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        return false;
    }

    // Folosim SQLITE_TRANSIENT pentru a forța SQLite să copieze datele
    sqlite3_bind_text(stmt, 1, j["email"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, j["password"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, j["nume"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, j["masina"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, j["numar_masina"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, j["user_id"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);

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
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            result["status"] = "success";
            result["email"] = (const char *)sqlite3_column_text(stmt, 0);
            result["nume"] = (const char *)sqlite3_column_text(stmt, 2);
            result["masina"] = (const char *)sqlite3_column_text(stmt, 3);
            result["numar_masina"] = (const char *)sqlite3_column_text(stmt, 4);
            result["user_id"] = (const char *)sqlite3_column_text(stmt, 5);
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::add_accident(double lat, double lng, const std::string &strada, const std::string &detalii)
{
    std::string sql = "INSERT INTO accidents (lat, long, strada, detalii, limita_recomandata) VALUES (?, ?, ?, ?, 30);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK)
    {
        std::cerr << "Eroare la preparare insert accidents: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // "Bind" la parametri (înlocuim semnele de întrebare cu datele reale)
    sqlite3_bind_double(stmt, 1, lat);
    sqlite3_bind_double(stmt, 2, lng);
    sqlite3_bind_text(stmt, 3, strada.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, detalii.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (success)
    {
        std::cout << "[DB] Accident stocat: " << strada << " (" << lat << ", " << lng << ")\n";
    }
    return success;
}

json DatabaseManager::get_speed_limit_at_location(double lat, double lng)
{
    // 1. Verificăm ACCIDENTE (rază mică: ~500m sau 0.005 unități)
    const char *sql_acc = "SELECT strada FROM accidents WHERE ABS(lat - ?) < 0.005 AND ABS(long - ?) < 0.005 LIMIT 1;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql_acc, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_double(stmt, 1, lat);
        sqlite3_bind_double(stmt, 2, lng);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::string s = (const char *)sqlite3_column_text(stmt, 0);
            sqlite3_finalize(stmt);
            return {{"limit", 30}, {"reason", "Zonă Accident: " + s}}; // Prioritate maximă
        }
    }
    sqlite3_finalize(stmt);

    // 2. Verificăm ORAȘE (folosim raza salvată în DB)
    const char *sql_city = "SELECT nume, limita FROM cities WHERE "
                           "((lat-?)*(lat-?) + (long-?)*(long-?)) < (raza*raza) LIMIT 1;";
    if (sqlite3_prepare_v2(db, sql_city, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_double(stmt, 1, lat);
        sqlite3_bind_double(stmt, 2, lat); // lat-?^2
        sqlite3_bind_double(stmt, 3, lng);
        sqlite3_bind_double(stmt, 4, lng);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::string nume = (const char *)sqlite3_column_text(stmt, 0);
            int lim = sqlite3_column_int(stmt, 1);
            sqlite3_finalize(stmt);
            return {{"limit", lim}, {"reason", "Intrare în oraș: " + nume}};
        }
    }
    sqlite3_finalize(stmt);

    // 3. Implicit: În afara orașului
    return {{"limit", 90}, {"reason", "În afara localității"}};
}

json DatabaseManager::get_all_accidents()
{
    json list = json::array();
    const char *sql = "SELECT lat, long, strada, detalii FROM accidents;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            json acc;
            acc["lat"] = sqlite3_column_double(stmt, 0);
            acc["long"] = sqlite3_column_double(stmt, 1);
            acc["locatie"] = (const char *)sqlite3_column_text(stmt, 2);
            acc["mesaj"] = (const char *)sqlite3_column_text(stmt, 3);
            list.push_back(acc);
        }
    }
    sqlite3_finalize(stmt);
    return list;
}