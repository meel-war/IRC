#include "Client.hpp"

Client::Client(int fd)
{
    _client_fd = fd;
    _nickname = "";
    _clientname = "";
    _has_pass = 0;
    _has_nick = 0;
    _has_client = 0;
}

void Client::joinChannel(Channel *channel)
{
    _joinedChannels.insert(channel); 
}

const std::string& Client::getNickname() const
{
    return (_nickname);
}

const std::string& Client::getClientname() const
{
    return (_clientname);
}

void Client::setNickname(std::string& nickname)
{
    _nickname = nickname;
}

void Client::setClientname(std::string& clientname)
{
    _clientname = clientname;
}

bool Client::hasPass() const
{
    return _has_pass;
}

bool Client::hasNick() const
{
    return _has_nick;
}

bool Client::hasClient() const
{
    return _has_client;
}
