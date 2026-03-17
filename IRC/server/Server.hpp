#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include "../client/Client.hpp"
#include "../channel/Channel.hpp"
#include <map>
#include <sstream>
#include <algorithm>

class Server {
    private:
        int _server_fd;
        std::string _password;
        std::vector<pollfd> _fds;
        std::vector<Client*> _clients;
        std::vector<Channel*> _channels;
        std::map<std::string, void(Server::*)(std::vector<std::string>, Client*)> _commands;

    public:
        Server(const std::string &port, const std::string &password);
        ~Server();
        void pollClients();
        void acceptClient();
        bool returnClient(const int client_fd);
        Client* getClientByFd(const int fd);
        void removeClient(const int fd);
        Channel* getChannelByName(const std::string &name);
        Channel* createChannel(const std::string &name);
		Client* getClientByName(const std::string &name);


        /* COMMANDES */
        void init_commands();
        void parse_commands(std::string message, Client* sender);
        void quit_com(std::vector<std::string> args, Client* sender);
        void part_com(std::vector<std::string> args, Client* sender);
        void nick_com(std::vector<std::string> args, Client* sender);
		void privmsg_com(std::vector<std::string> args, Client* sender);
		void user_com(std::vector<std::string> args, Client* sender);
		void pass_com(std::vector<std::string> args, Client* sender);
        void invite_com(std::vector<std::string> args, Client* sender);
        void kick_com(std::vector<std::string> args, Client* sender);
        void topic_com(std::vector<std::string> args, Client* sender);
        void mode_com(std::vector<std::string> args, Client* sender);
		void join_com(std::vector<std::string> args, Client* sender);
		void ping_com(std::vector<std::string> args, Client* sender);
};

std::vector<std::string> split(std::string s, char c);
void sendMsg(const int& fd, const std::string& msg);