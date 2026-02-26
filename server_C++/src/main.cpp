#include <iostream>
#include "TCPServer.hpp"
#include "TrafficManager.hpp"

int main() {
    try
    {
        //business logic (cel care procesează mesajele)
        TrafficManager trafficManager;

        //serverul pe portul 1111 și managerul ca referinta
        TCPServer server(1111, trafficManager);

        std::cout << "[SYSTEM] Serverul de monitorizare trafic se inițializează..." << '\n';

        //(aici va rula loop-ul cu select())
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[CRITICAL ERROR] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}