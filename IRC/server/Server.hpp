#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include "../client/Client.hpp"

class Server {

    private:
        int _server_fd;
        std::vector<pollfd> _fds;
        std::vector<Client> _clients;

    public:
        Server(const std::string &port, const std::string &password);
        ~Server();
        void pollClients();
        void acceptClient();
        void returnClient(int client_fd);

};