#include <iostream>
#include <atomic>
#include <csignal>

#include "TCPServer.hpp"
#include "TrafficManager.hpp"

// semnal adaugat pentru momentul in care se inchide serverul fortat, pentru activarea destructorului bazei de date (stergerea accidentelor)
std::atomic<bool> keep_running(true);

void signal_handler(int signal)
{
    std::cout << "\n[SYSTEM] Semnal de oprire primit (" << signal << "). Închidem serverul...\n";
    keep_running = false;
}

int main()
{
    std::signal(SIGINT, signal_handler);

    try
    {
        // business logic (cel care procesează mesajele)
        TrafficManager trafficManager;

        // serverul pe portul 1111 și managerul ca referinta
        TCPServer server(1111, trafficManager);

        //(aici va rula loop-ul cu select())
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[CRITICAL ERROR] " << e.what() << '\n';
        return 1;
    }

    return 0;
}