#include "TrafficManager.hpp"

void TrafficManager::proceseaza_mesaj(int client_socket,
                                      const std::string &raw_data,
                                      const std::vector<int> &toti_clientii)
{
    try
    {
        auto j = json::parse(raw_data);
        std::string type = j["type"];

        if (j["type"] == "signup")
        {
            if (db.add_user(j))
            {
                trimite_raspuns(client_socket,
                                {{"status", "ok"}, {"msg", "Cont creat!"}});
            }
        }
        else if (j["type"] == "login")
        {
            json auth = db.authenticate_user(j["email"], j["password"]);
            if (auth["status"] == "success")
            {
                auth["type"] = "auth_response";
                trimite_raspuns(client_socket, auth);
            }
            else
            {
                trimite_raspuns(client_socket,
                                {{"type", "auth_response"}, {"status", "fail"}});
            }
        }
        else if (type == "speed_update")
        {
            handle_speed_update(client_socket, j);
        }
        else if (type == "incident")
        {
            handle_incident(client_socket, j, toti_clientii);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Eroare Business Logic: " << e.what() << std::endl;
    }
}

void TrafficManager::handle_login_auth(int socket, const json &j)
{
    std::string email = j["email"];
    std::string password = j["password"];

    // Pseudo-cod SQL: SELECT * FROM users WHERE email='...' AND password='...'
    // Daca gasim userul:
    json response = {{"type", "auth_success"},
                     {"user_id", "25"},
                     {"nume", "Marian"},
                     {"masina", "BMW"},
                     {"numar_masina", "IS 02 MSU"}};
    trimite_raspuns(socket, response);
}

void TrafficManager::handle_speed_update(int socket, const json &j)
{
    if (participanti.count(socket))
    {
        participanti[socket].ultima_viteza = j["value"];
        std::cout << "[SPEED] " << participanti[socket].numar_masina << ": "
                  << j["value"] << " km/h\n";

        // Logică cerută: Dacă viteza e prea mică, notificăm ceilalți de un posibil
        // blocaj
        if (j["value"] < 10)
        {
            json alert = {{"type", "info"},
                          {"msg", "Trafic intens detectat in zona!"}};
            // Aici s-ar putea face un broadcast selectiv
        }
    }
}

void TrafficManager::handle_incident(int socket, const json &j,
                                     const std::vector<int> &toti_clientii)
{
    // Cerință: Un client anunță un accident, serverul trimite la toți
    std::cout << "[INCIDENT] Raportat de " << participanti[socket].numar_masina
              << '\n';

    json update = {{"type", "broadcast_incident"},
                   {"locatie", j["locatie"]},
                   {"detalii", j["mesaj"]}};

    broadcast(toti_clientii, update);
}

void TrafficManager::trimite_raspuns(int socket, const json &j)
{
    std::string s = j.dump() + "\n";
    send(socket, s.c_str(), s.length(), 0);
}

void TrafficManager::broadcast(const std::vector<int> &receptori, const json &j)
{
    for (int socket : receptori)
    {
        trimite_raspuns(socket, j);
    }
}