#include "TrafficManager.hpp"

void TrafficManager::proceseaza_mesaj(int client_socket, const std::string &raw_data, const std::vector<int> &toti_clientii) {
    try {
        auto j = json::parse(raw_data);
        std::string type = j["type"];

        if (type == "login") {
            handle_login(client_socket, j);
        }
        else if (type == "speed_update") {
            handle_speed_update(client_socket, j);
        }
        else if (type == "incident") {
            handle_incident(client_socket, j, toti_clientii);
        }
    }
    catch (std::exception &e) {
        std::cerr << "Eroare Business Logic: " << e.what() << std::endl;
    }
}

void TrafficManager::handle_login(int socket, json &j) {
    Client c;
    c.socket_fd = socket;
    c.user_id = j["user_id"];
    c.nume = j["nume"];
    c.masina = j["masina"];
    c.numar_masina = j["numar_masina"];

    participanti[socket] = c;
    std::cout << "[LOG]: Soferul " << c.nume << " s-a înregistrat.\n";

    json resp = {{"status", "ok"}, {"msg", "Bun venit in sistem!"}};
    trimite_raspuns(socket, resp);
}

void TrafficManager::handle_speed_update(int socket, json &j) {
    if (participanti.count(socket))
    {
        participanti[socket].ultima_viteza = j["value"];
        std::cout << "[SPEED] " << participanti[socket].numar_masina << ": " << j["value"] << " km/h\n";

        // Logică cerută: Dacă viteza e prea mică, notificăm ceilalți de un posibil blocaj
        if (j["value"] < 10)
        {
            json alert = {{"type", "info"}, {"msg", "Trafic intens detectat in zona!"}};
            // Aici s-ar putea face un broadcast selectiv
        }
    }
}

void TrafficManager::handle_incident(int socket, json &j, const std::vector<int> &toti_clientii){
    // Cerință: Un client anunță un accident, serverul trimite la toți
    std::cout << "[INCIDENT] Raportat de " << participanti[socket].numar_masina << '\n';

    json update = {
        {"type", "broadcast_incident"},
        {"locatie", j["locatie"]},
        {"detalii", j["mesaj"]}};

    broadcast(toti_clientii, update);
}

void TrafficManager::trimite_raspuns(int socket, json &j) {
    std::string s = j.dump() + "\n";
    send(socket, s.c_str(), s.length(), 0);
}

void TrafficManager::broadcast(const std::vector<int> &receptori, json &j){
    for (int socket : receptori)
    {
        trimite_raspuns(socket, j);
    }
}
