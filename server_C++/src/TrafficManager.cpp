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

}

void TrafficManager::handle_speed_update(int socket, json &j) {

}

void TrafficManager::handle_incident(int socket, json &j, const std::vector<int> &toti_clientii){

}

void TrafficManager::trimite_raspuns(int socket, json &j) {

}

void TrafficManager::broadcast(const std::vector<int> &receptori, json &j){

}
