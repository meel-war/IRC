#include "Client.hpp"

Client::Client(int fd)
    : _client_fd(fd), _nickname(""), _clientname(""), _realname(""), _has_pass(false), _has_nick(false), _has_client(false), _has_realname(false) {}

void Client::joinChannel(Channel *channel)
{
    _joinedChannels.insert(channel); 
}

const std::string& Client::getNickname() const
{
    return (_nickname);
}

const int& Client::getFd() const
{
    return(_client_fd);
}

const std::string& Client::getClientname() const
{
    return (_clientname);
}

void Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
}

void Client::setClientname(const std::string& clientname)
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

void Client::setHasPass(bool pass)
{
    _has_pass = pass;
}

void Client::setHasNick(bool nick)
{
    _has_nick = nick;
}

void Client::setHasClient(bool client)
{
    _has_client = client;
}

void Client::appendBuffer(const std::string &new_buffer)
{
    _buffer += new_buffer;
}

std::string& Client::getBuffer()
{
    return(_buffer);
}

std::string& Client::getRealName()
{
    return(_realname);
}

bool Client::hasRealName() const {
	return (_has_realname);
}

void Client::setHasRealName(bool pass) {
	_has_realname = pass;
}

void Client::setRealName(const std::string realname) {
	_realname = realname;
}