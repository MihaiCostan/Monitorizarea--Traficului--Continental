#pragma once
#include <sqlite3.h>
#include <sys/socket.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "DatabaseManager.hpp"
#include "json.hpp"
using json = nlohmann::json;

struct Client
{
    int socket_fd;
    std::string user_id;
    std::string nume;
    std::string masina;
    std::string numar_masina;
    std::string email;
    int ultima_viteza = 0;

    bool vrea_vreme = false;
    bool vrea_sport = false;
    bool vrea_peco = false;
};

class TrafficManager
{
private:
    std::map<int, Client> participanti;
    DatabaseManager db;

public:
    TrafficManager() : db("traffic.db") {}
    void proceseaza_mesaj(int client_socket, const std::string &raw_data,
                          const std::vector<int> &toti_clientii);

private:
    void handle_login(int socket, const json &j);
    void handle_actual_speed_update_and_infotaiment(int socket, const json &j);
    void handle_accident(int socket, const json &j,
                         const std::vector<int> &toti_clientii);
    void handle_update_preferences(int client_socket, json &j);

    // Helper pentru trimiterea de mesaje JSON
    void broadcast_single_client(int socket, const json &j);
    void broadcast_all_clients(const std::vector<int> &receptori, const json &j);
    void broadcast_except_sender(const std::vector<int> &receptori, const json &j, int sender_socket);

    // Validator login
    void handle_login_auth(int socket, const json &j);
};