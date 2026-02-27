#include "TrafficManager.hpp"

void TrafficManager::proceseaza_mesaj(int client_socket,
                                      const std::string &raw_data,
                                      const std::vector<int> &toti_clientii)
{
    try
    {
        auto j = json::parse(raw_data);
        std::string type = j["type"];

        if (type == "signup")
        {
            if (db.add_user(j))
            {
                trimite_raspuns(client_socket, {{"type", "signup_response"}, // Adăugat pentru sincronizare
                                                {"status", "success"},       // Schimbat din "ok" în "success" pentru a bate cu Python
                                                {"msg", "Cont creat cu succes!"}});
            }
            else
            {
                trimite_raspuns(client_socket, {{"type", "signup_response"},
                                                {"status", "error"},
                                                {"msg", "Eroare: Email-ul există deja!"}});
            }
        }
        else if (type == "login")
        {
            json auth = db.authenticate_user(j["email"], j["password"]);
            auth["type"] = "login_response";

            if (auth["status"] == "success")
            {
                // salvez clientul in RAM
                Client c;
                c.socket_fd = client_socket;
                c.nume = auth["nume"];
                c.masina = auth["masina"];
                c.numar_masina = auth["numar_masina"];
                c.user_id = auth["user_id"];

                participanti[client_socket] = c;
                trimite_raspuns(client_socket, auth);
            }
            else
            {
                trimite_raspuns(client_socket, {{"type", "login_response"}, {"status", "fail"}});
            }
        }
        else if (type == "speed_update")
        {
            handle_speed_update(client_socket, j);
        }
        else if (type == "accident")
        {
            handle_accident(client_socket, j, toti_clientii);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Eroare Business Logic: " << e.what() << std::endl;
    }
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

void TrafficManager::handle_accident(int socket, const json &j,
                                     const std::vector<int> &toti_clientii)
{
    if (participanti.count(socket))
    {
        std::cout << "[ACCIDENT] Raportat de " << participanti[socket].numar_masina << '\n';
    }

    json update = {
        {"type", "broadcast_accident"},
        {"locatie", j["locatie"]},
        {"mesaj", j["mesaj"]}, // Sincronizat cu Python
        {"lat", j["lat"]},     // Adăugat pentru hartă
        {"long", j["long"]}    // Adăugat pentru hartă
    };

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