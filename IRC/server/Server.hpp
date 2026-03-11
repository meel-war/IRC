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
        std::map<std::string, void (Server::*)(std::string, Client&)> _commands;

    public:
        Server(const std::string &port, const std::string &password);
        ~Server();
        void pollClients();
        void acceptClient();
        bool returnClient(int client_fd);
        Client& getClientByFd(int fd);
        void parser(Client& sender, std::string raw);


        /* COMMANDES */
        void init_commands();
        void parse_commands(std::string message, Client& sender);
        void quit_com(std::vector<std::string> args, Client& sender);
        void part_com(std::vector<std::string> args, Client& sender);
        void nick_com(std::vector<std::string> args, Client& sender);
};

std::vector<std::string> split(std::string s, std::string delimiter);