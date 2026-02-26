#include "User.hpp"

User::User(int fd)
{
    _client_fd = fd;
    _nickname = "";
    _username = "";
    _has_pass = 0;
    _has_nick = 0;
    _has_user = 0;
}

void User::joinChannel(Channel *channel)
{
    _joinedChannels.insert(channel); 
}

const std::string& User::getNickname() const
{
    return (_nickname);
}

const std::string& User::getUsername() const
{
    return (_username);
}

void User::setNickname(std::string& nickname)
{
    _nickname = nickname;
}

void User::setUsername(std::string& username)
{
    _username = username;
}

bool User::hasPass() const
{
    return _has_pass;
}

bool User::hasNick() const
{
    return _has_nick;
}

bool User::hasUser() const
{
    return _has_user;
}
