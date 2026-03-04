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
                broadcast_single_client(client_socket, {{"type", "signup_response"}, // Adăugat pentru sincronizare
                                                        {"status", "success"},       // Schimbat din "ok" în "success" pentru a bate cu Python
                                                        {"msg", "Cont creat cu succes!"}});
            }
            else
            {
                broadcast_single_client(client_socket, {{"type", "signup_response"},
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
                c.email = auth["email"];

                participanti[client_socket] = c;

                std::cout << "[SERVER]: Client details: [socket: " << client_socket << "], [name: " << c.nume << "], [email: " << c.email << "]\n";
                broadcast_single_client(client_socket, auth);

                // pentru incarcarea markerelor de la accidentele actuale de pe server
                json initial_accidents = {
                    {"type", "initial_accidents_sync"},
                    {"data", db.get_all_accidents()}};
                broadcast_single_client(client_socket, initial_accidents);
            }
            else
            {
                broadcast_single_client(client_socket, {{"type", "login_response"}, {"status", "fail"}});
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
        double lat = j["lat"];
        double lng = j["long"];
        participanti[socket].ultima_viteza = j["value"];

        // Calculăm limita de viteză curentă bazată pe locație
        json advisory = db.get_speed_limit_at_location(lat, lng);
        advisory["type"] = "speed_limit_advisory";

        // Trimitem răspunsul DOAR clientului care a făcut update-ul
        broadcast_single_client(socket, advisory);
    }
}

void TrafficManager::handle_accident(int socket, const json &j,
                                     const std::vector<int> &toti_clientii)
{
    if (db.add_accident(j["lat"], j["long"], j["locatie"], j["mesaj"]))
    {
        json update = {
            {"type", "accident"},
            {"locatie", j["locatie"]},
            {"nume", participanti[socket].nume},
            {"mesaj", j["mesaj"]}, // Sincronizat cu Python
            {"lat", j["lat"]},     // Adăugat pentru hartă
            {"long", j["long"]},   // Adăugat pentru hartă
            {"sender", participanti[socket].numar_masina}};

        broadcast_all_clients(toti_clientii, update);
    }
    else
        std::cout << "Eroare la inserarea accidentului in baza de date!\n";
}

void TrafficManager::broadcast_single_client(int socket, const json &j)
{
    std::string s = j.dump() + "\n";
    send(socket, s.c_str(), s.length(), 0);
}

void TrafficManager::broadcast_all_clients(const std::vector<int> &receptori, const json &j)
{
    for (int socket : receptori)
    {
        broadcast_single_client(socket, j);
    }
}

void TrafficManager::broadcast_except_sender(const std::vector<int> &receptori, const json &j, int sender_socket)
{
    for (int socket_client : receptori)
    {
        if (socket_client != sender_socket)
        {
            broadcast_single_client(socket_client, j);
        }
    }
}