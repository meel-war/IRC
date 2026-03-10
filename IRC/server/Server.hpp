#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
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
        bool returnClient(int client_fd);
        Client& getClientByFd(int fd);
        void parser(Client& sender, std::string raw);
};