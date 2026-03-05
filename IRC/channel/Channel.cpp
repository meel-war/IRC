#include "Channel.hpp"

void Channel::removeClient(Client *client)
{
    _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
    _invited.erase(client);
    _operators.erase(client);
}

void Channel::addClient(Client *client)
{
    if (!isInChannel(client)) {
        if ((!hasMode('l') || !isFull()) && 
            (!hasMode('i') || isInvited(client))) {
                _clients.push_back(client);
                client->joinChannel(this);
                _invited.erase(client);
        }
    }
}

bool Channel::isInChannel(Client *client) const
{
    if (std::find(_clients.begin(), _clients.end(), client) == _clients.end()) {
        return false;
    }
    return true;
}

void Channel::addOperator(Client *client)
{
    if (isInChannel(client))
    {
        _operators.insert(client);
    }
}

bool Channel::isOperator(Client *client) const
{
    if (_operators.find(client) != _operators.end())
        return true;
    return false;
}

void Channel::removeOperator(Client *client)
{
    _operators.erase(client);
}

void Channel::addMode(char mode)
{
    _modes.insert(mode);
}

bool Channel::hasMode(char mode) const
{
    if(_modes.find(mode) != _modes.end())
        return true;
    return false;
}

void Channel::removeMode(char mode)
{
    _modes.erase(mode);
}

void Channel::setKey(const std::string &key)
{
    _key = key;
}

void Channel::removeKey()
{
    _key = "";
}

bool Channel::checkKey(const std::string &input) const
{
    if (input == _key)
        return true;
    return false;
}

void Channel::setClientLimit(int limit)
{
    _clientLimit = limit;
}

void Channel::removeClientLimit()
{
    _clientLimit = 0;
}

bool Channel::isFull() const
{
    if (_clientLimit == 0)
        return false;
    return static_cast<int>(_clients.size()) >= _clientLimit;
}

void Channel::inviteClient(Client* client)
{
    if (!isInChannel(client))
        _invited.insert(client);
}

bool Channel::isInvited(Client* client)
{
    if (_invited.find(client) != _invited.end())
        return true;
    return false;
}

void Channel::setTopic(const std::string& topic, Client *setter)
{
    if (hasMode('t')) {
        if (isInChannel(setter) && isOperator(setter))
            _topic = topic;
		//mettre message d'erreur pour celui qui a fait la commande s'il n'est pas operateur donc tu codes ca sur le champ stp
    }
    else if (isInChannel(setter))
        _topic = topic;
}

std::string Channel::getModes() const
{
    std::string actif_modes;

    actif_modes += '+';
    std::copy(_modes.begin(), _modes.end(), std::back_inserter(actif_modes));

    return actif_modes;
}

const std::vector<Client *>& Channel::getClients() const
{
    return (_clients);
}

const std::string& Channel::getTopic() const
{
    return (_topic);
}

Client *Channel::findClientByNickname(const std::string &nickname) const
{
    int i;
    for(i = 0; i < _clients.size(); i++) {
        Client* client = _clients[i];
        if (client->getNickname() == nickname)
            return client;
    }
    return NULL;
}

void Channel::broadcast(Client *sender, const std::string &message)
{
    for(int i = 0; i < _clients.size(); i++) {
        if (sender != _clients[i])
            // _client[i]->sendMessage(message);
            _clients[i]; // a supprimer 
    }
}
