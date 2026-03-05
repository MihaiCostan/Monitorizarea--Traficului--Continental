#pragma once
#include <sqlite3.h>
#include <sys/socket.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "DatabaseManager.hpp"
#include "json.hpp"
using json = nlohmann::json; // transmitere date prin simulare json

struct Client
{
    int socket_fd;
    std::string user_id;
    std::string nume;
    std::string masina;
    std::string numar_masina;
    std::string email;
    int ultima_viteza = 0;

    int vrea_vreme = 0;
    int vrea_sport = 0;
    int vrea_peco = 0;
};

class TrafficManager
{
private:
    std::map<int, Client> clients; // pentru acces rapid via RAM
    DatabaseManager db;

public:
    TrafficManager() : db("traffic.db") {}
    void proceseaza_mesaj(int client_socket, const std::string &raw_data,
                          const std::vector<int> &all_clients_sockets);

private:
    void handle_login(int client_socket, const json &j);
    void handle_signup(int client_socket, const json &j);
    void handle_get_limit(int client_socket, const json &j);
    void handle_actual_speed_update_and_infotaiment(int client_socket, const json &j);
    void handle_update_preferences(int client_socket, json &j);
    void handle_accident(int client_socket, const json &j,
                         const std::vector<int> &all_clients_sockets);

    // Helper pentru trimiterea de mesaje JSON
    void broadcast_single_client(int client_socket, const json &j);
    void broadcast_all_clients(const std::vector<int> &all_clients_sockets, const json &j);
};