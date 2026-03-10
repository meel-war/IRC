#include "Server.hpp"

Server::Server(const std::string &port, const std::string &password)
{
    (void)password;
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_server_fd < 0)
    {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET; // IPV4
    addr.sin_addr.s_addr = INADDR_ANY; // Ecoute toutes les interfaces reseau : localhost/wifi/ethernet
    addr.sin_port = htons(std::stoi(port.c_str())); // (htons)Conversion en network byte order pour bind

    if(bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) // socket_serv prend les attributs de addr
    {
        perror("bind");
        throw std::runtime_error("Bind failed");
    }

    if(listen(_server_fd, 4096) < 0) // met le socket_serv en ecoute/passif
    {
        perror("listen");
        throw std::runtime_error("Listen failed");
    }

    pollfd server_poll;
    server_poll.fd = _server_fd;
    server_poll.events = POLLIN;
    _fds.push_back(server_poll);

    std::cout << "Server listening on port " << port << std::endl;
}

void Server::pollClients()
{
    if(poll(_fds.data(), _fds.size(), -1) < 0)
    {
        perror("poll");
        return ;
    }

    for(size_t i = 0; i < _fds.size(); i++)
    {
        if(_fds[i].revents & POLLIN)
        {
            if(_fds[i].fd == _server_fd)
            {
                acceptClient(); // nouveau client, ajouter poll_fd
            }
            else
            {
                if(!returnClient(_fds[i].fd)) // client existant, msg
                {
                    close(_fds[i].fd);
                    _fds.erase(_fds.begin() + i);
                    i--;
                }
            }
        }
    }
}

void Server::acceptClient()
{
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if(client_fd < 0)
        throw std::runtime_error("Accept failed");
    Client new_client(client_fd);
    _clients.push_back(new_client);

    //ajout dans le tableau des fds_poll en msg entrant
    pollfd new_client_poll;
    new_client_poll.fd = client_fd;
    new_client_poll.events = POLLIN;
    _fds.push_back(new_client_poll);

    std::cout << "New client connected: fd " << client_fd << std::endl;
}

bool Server::returnClient(int client_fd)
{
    char buffer[1025];
    int bytes = recv(client_fd, buffer, 1024, 0);
    if(bytes <= 0)
    {
        std::cout << "Client disconnected: fd " << client_fd << std::endl;
        return (false);
    }
    else
    {
        buffer[bytes] = '\0';
        std::string raw(buffer);
        std::cout << "RAW: [" << buffer << "]" << std::endl; // DEBUG
        Client& sender = getClientByFd(client_fd);
        parser(sender, raw);
        return (true);
    }
}

void Server::parser(Client& sender, std::string raw)
{
    (void)sender;
    std::cout << raw << " : ";
}

Client& Server::getClientByFd(int fd)
{
    for(size_t i = 0; i < _clients.size(); i++)
        if(_clients[i].getFd() == fd)
            return(_clients[i]);
    throw std::runtime_error("Client not found"); // a changer surement
}

Server::~Server() {}
