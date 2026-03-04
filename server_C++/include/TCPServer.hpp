#pragma once
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <atomic>

#include "TrafficManager.hpp"

#define BUFFER_SIZE 2048

class TCPServer
{
private:
    int server_fd;
    std::vector<int> clients_sockets;
    TrafficManager &manager;

public:
    TCPServer(int port, TrafficManager &manager);
    ~TCPServer();
    void run();
};