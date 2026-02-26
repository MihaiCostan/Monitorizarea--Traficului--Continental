#include "TCPServer.hpp"

TCPServer::TCPServer(int port, TrafficManager &manager) : manager(manager)
{
    struct sockaddr_in address;
    int opt = 1;

    // initializare socket
    if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "Socket failed");
        exit(EXIT_FAILURE);
    }

    // aplicare optiuni
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0)
    {
        fprintf(stderr, "SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }

    // initializare adresa
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind & listen
    if (bind(this->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "Bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(this->server_fd, 10) < 0)
    {
        fprintf(stderr, "Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER]: Server started on port %d\n", port);
}

void TCPServer::run()
{
    printf("[SERVER]: Server waiting for connections...\n");
    fd_set readfds;
    int sd, max_sd, activity, new_socket;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (int client_sd : clients_sockets)
        {
            if (client_sd > 0)
                FD_SET(client_sd, &readfds);
            if (client_sd > max_sd)
                max_sd = client_sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            fprintf(stderr, "Select error");
        }

        // a. conexiune noua
        if (FD_ISSET(server_fd, &readfds))
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr,
                                     &client_len)) < 0)
            {
                fprintf(stderr, "Accept error");
                exit(EXIT_FAILURE);
            }

            printf("[SERVER] Conexiune noua! Socket: %d\n", new_socket);
            clients_sockets.push_back(new_socket);
        }

        // b. mesaje din partea clientilor
        for (auto it = clients_sockets.begin(); it != clients_sockets.end();)
        {
            sd = *it;
            if (FD_ISSET(sd, &readfds))
            {
                int valread = read(sd, buffer, 2048);
                if (valread == 0)
                {
                    // Client deconectat
                    printf("[SERVER] Client deconectat: %d\n", sd);
                    close(sd);
                    it = clients_sockets.erase(it); // Ștergem din vector
                    continue;
                }
                else
                {
                    buffer[valread] = '\0';
                    manager.proceseaza_mesaj(sd, std::string(buffer), clients_sockets);
                }
            }
            ++it;
        }
    }
}