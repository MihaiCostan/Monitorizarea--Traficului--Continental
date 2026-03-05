#include "TrafficManager.hpp"

void TrafficManager::proceseaza_mesaj(int client_socket,
                                      const std::string &raw_data,
                                      const std::vector<int> &all_clients_sockets)
{
    try
    {
        auto j = json::parse(raw_data);
        std::string type = j["type"];

        if (type == "signup")
        {
            handle_signup(client_socket, j);
        }
        else if (type == "login")
        {
            handle_login(client_socket, j);
        }
        else if (type == "get_limit")
        {
            handle_get_limit(client_socket, j);
        }
        else if (type == "actual_speed")
        {
            handle_actual_speed_update_and_infotaiment(client_socket, j);
        }
        else if (type == "update_preferences")
        {
            handle_update_preferences(client_socket, j);
        }
        else if (type == "accident")
        {
            handle_accident(client_socket, j, all_clients_sockets);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Eroare Business Logic: " << e.what() << std::endl;
    }
}

void TrafficManager::handle_signup(int client_socket, const json &j)
{
    if (db.add_user(j))
    {
        broadcast_single_client(client_socket, {{"type", "signup_response"},
                                                {"status", "success"},
                                                {"msg", "Cont creat cu succes!"}});
    }
    else
    {
        broadcast_single_client(client_socket, {{"type", "signup_response"},
                                                {"status", "error"},
                                                {"msg", "Eroare la crearea contului!"}});
    }
}

void TrafficManager::handle_login(int client_socket, const json &j)
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

        clients[client_socket] = c;

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

void TrafficManager::handle_get_limit(int client_socket, const json &j)
{
    if (clients.count(client_socket))
    {
        double lat = j["lat"];
        double lng = j["long"];

        // Cerem limita din DB fără a modifica datele participantului
        json advisory = db.get_speed_limit_at_location(lat, lng);
        advisory["type"] = "zone_limit_update";

        broadcast_single_client(client_socket, advisory);
    }
}

void TrafficManager::handle_actual_speed_update_and_infotaiment(int client_socket, const json &j)
{
    if (clients.count(client_socket))
    {
        double lat = j["lat"];
        double lng = j["long"];
        clients[client_socket].ultima_viteza = j["value"];

        // Calculăm limita de viteză curentă bazată pe locație
        json advisory = db.get_speed_limit_at_location(lat, lng);
        advisory["type"] = "actual_speed";

        advisory["infotainment_news"] = db.get_infotaiment(clients[client_socket].email);

        // Trimitem răspunsul DOAR clientului care a făcut update-ul
        broadcast_single_client(client_socket, advisory);
    }
}

void TrafficManager::handle_update_preferences(int client_socket, json &j)
{
    if (clients.count(client_socket))
    {
        // Actualizăm în RAM pentru utilizare imediată
        clients[client_socket].vrea_vreme = j["vreme"];
        clients[client_socket].vrea_sport = j["sport"];
        clients[client_socket].vrea_peco = j["peco"];

        // Actualizăm permanent în Baza de Date
        db.update_user_preferences(
            clients[client_socket].email,
            j["vreme"], j["sport"], j["peco"]);

        std::cout << "[SERVER]: Preferințe infotainment actualizate pentru " << clients[client_socket].nume << "\n";
    }
}

void TrafficManager::handle_accident(int client_socket, const json &j,
                                     const std::vector<int> &all_clients_sockets)
{
    if (db.add_accident(j["lat"], j["long"], j["locatie"], j["mesaj"]))
    {
        json update = {
            {"type", "accident"},
            {"locatie", j["locatie"]},
            {"nume", clients[client_socket].nume},
            {"mesaj", j["mesaj"]}, // Sincronizat cu Python
            {"lat", j["lat"]},     // Adăugat pentru hartă
            {"long", j["long"]},   // Adăugat pentru hartă
            {"sender", clients[client_socket].numar_masina}};

        broadcast_all_clients(all_clients_sockets, update);
    }
    else
        std::cout << "Eroare la inserarea accidentului in baza de date!\n";
}

void TrafficManager::broadcast_single_client(int client_socket, const json &j)
{
    std::string s = j.dump() + "\n";
    send(client_socket, s.c_str(), s.length(), 0);
}

void TrafficManager::broadcast_all_clients(const std::vector<int> &all_clients_sockets, const json &j)
{
    for (int socket : all_clients_sockets)
    {
        broadcast_single_client(socket, j);
    }
}