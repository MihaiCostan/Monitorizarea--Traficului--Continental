#include "include/TCPServer.hpp"

TCPServer::TCPServer(int port, TrafficManager& manager) : manager(manager){
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // initializare socket
    if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Socket failed");
        exit(EXIT_FAILURE);
    }

    // aplicare optiuni
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }

    // initializare adresa
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind & listen
    if (bind(this->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(this->server_fd, 10) < 0) {
        fprintf(stderr, "Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server started on port %d", port);
}

void TCPServer::run() {

}