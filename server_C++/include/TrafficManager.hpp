#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

struct Client{
    int socket_fd;
    std::string user_id;
    std::string nume;
    std::string masina;
    std::string numar_masina;
    int ultima_viteza = 0;

    bool vrea_vreme = false;
    bool vrea_sport = false;
    bool vrea_peco = false;
};

class TrafficManager {
    private:
        std::map<int, Client> participanti;
    
    public:
        void proceseaza_mesaj(int client_socket, const std::string &raw_data, const std::vector<int> &toti_clientii);
    
    private:
        void handle_login(int socket, json &j);
        void handle_speed_update(int socket, json &j);
        void handle_incident(int socket, json &j, const std::vector<int> &toti_clientii);

        // Helper pentru trimiterea de mesaje JSON
        void trimite_raspuns(int socket, json &j);
        void broadcast(const std::vector<int> &receptori, json &j);
};