#include "Server.hpp"

Server::Server(const std::string &port, const std::string &password)
{
    char* endptr;
    long int port_num = strtol(port.c_str(), &endptr, 10);
    if(*endptr != '\0' || port_num < 1024 || port_num > 65535)
        throw std::runtime_error("Port must be a number between 1024 and 65535"); // Surement a rendre plus clean (temp)
    
	_name = "brainrot";
    _password = password;
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

    init_commands();
    _bot = NULL;
    _lastBotMsg = time(NULL);
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
    botMsg();
}

void Server::acceptClient()
{
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if(client_fd < 0)
        throw std::runtime_error("Accept failed");
    Client* new_client = new Client(client_fd);
    _clients.push_back(new_client);

    //ajout dans le tableau des fds_poll en msg entrant
    pollfd new_client_poll;
    new_client_poll.fd = client_fd;
    new_client_poll.events = POLLIN;
    _fds.push_back(new_client_poll);

    std::cout << "New client connected: fd " << client_fd << std::endl;
}

bool Server::returnClient(const int client_fd)
{
    char buffer[513];
    int bytes = recv(client_fd, buffer, 512, 0); // Norme offi IRC, un msg IRC limite a 512 bytes max
    if(bytes <= 0)
    {
        std::cout << "Client disconnected: fd " << client_fd << std::endl;
        removeClient(client_fd);
        return (false);
    }
    else
    {
        buffer[bytes] = '\0';
        Client* sender = getClientByFd(client_fd);
        if(!sender)
            return(false);
        sender->appendBuffer(buffer); // append la suite au buffer si le precedent msg ne se termine pas par \r\n
        std::string &clientBuffer = sender->getBuffer();
        size_t pos;

        while((pos = clientBuffer.find("\r\n")) != std::string::npos) //Trouve les \r\n dans le buffer
        {
            std::string command = clientBuffer.substr(0, pos);
            clientBuffer.erase(0, pos + 2);
            if(command.empty())
                continue;
            std::cout << "CMD: [" << command << "]" << std::endl;
            parse_commands(command, sender);
        }
        return (true);
    }
}

void Server::removeClient(const int fd)
{
    for(size_t i = 0; i < _clients.size(); i++)
    {
        if(_clients[i]->getFd() == fd)
        {
            Client* client = _clients[i];
            for(size_t j = 0; j < _channels.size(); j++)
            {
                _channels[j]->removeClient(client);
                if(_channels[j]->getClients().size() <= 1)
                {
                    delete _channels[j];
                    _channels.erase(_channels.begin() + j);
                    j--;
                }
            }
            delete client;
            _clients.erase(_clients.begin() + i);
            return;
        }
    }
}

Channel* Server::getChannelByName(const std::string &name)
{
    for(size_t i = 0; i < _channels.size(); i++)
    {
        if(_channels[i]->getName() == name)
            return(_channels[i]);
    }
    return(NULL);    
}

Client* Server::getClientByName(const std::string &name)
{
    for(size_t i = 0; i < _clients.size(); i++)
    {
        if(_clients[i]->getNickname() == name)
            return(_clients[i]);
    }
    return(NULL);    
}

Channel* Server::createChannel(const std::string &name)
{
    Channel* ch = getChannelByName(name);
    if(ch)
        return(ch);
    Channel* new_channel = new Channel(name);
    if(_bot)
    {
        new_channel->addClient(_bot);
        new_channel->addOperator(_bot);
    }
    _channels.push_back(new_channel);
    return(new_channel);
}

Client* Server::getClientByFd(const int fd)
{
    for(size_t i = 0; i < _clients.size(); i++)
        if(_clients[i]->getFd() == fd)
            return(_clients[i]);
    throw std::runtime_error("Client not found");
}

void Server::initBot()
{
    _bot = new Client(-1);
    _bot->setNickname("BOT");
    _bot->setClientname("BRAIN BOT");
    _bot->setHasPass(true);
    _bot->setHasNick(true);
    _bot->setHasClient(true);
}

void Server::botMsg()
{
    if(!_bot)
        return;
    
    time_t cur = time(NULL);

    if(cur - _lastBotMsg < 60)
        return;
    
    _lastBotMsg = cur;

    const char* messages[] = {
        "Noobini Pizzanini",
        "Trippi Troppi",
        "Cappuccino Assassino",
        "Ballerina Cappuccina",
        "Bombardiro Crocodilo",
        "Tralalero Tralala"
    };

    srand(cur);
    int idx = rand() % 6;
    std::string msg = messages[idx];

    for(size_t i = 0; i < _channels.size(); i++)
    {
        if(_channels[i]->getClients().size() > 1){
            std::string fullMsg = ":BOT!BRAINBOT@" + _name + " PRIVMSG " + _channels[i]->getName() + " :" + msg;
            _channels[i]->broadcast(_bot, fullMsg);
        }
    }
}

Server::~Server() 
{
    for(size_t i = 0; i < _clients.size(); i++)
        delete _clients[i];
    
    for(size_t i = 0; i < _channels.size(); i++)
        delete _channels[i];
    
    close(_server_fd);
}
