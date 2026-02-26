#include "Channel.hpp"

void Channel::removeUser(User *user)
{
    _users.erase(std::remove(_users.begin(), _users.end(), user), _users.end());
    _invited.erase(user);
    _operators.erase(user);
}

void Channel::addUser(User *user)
{
    if (!isInChannel(user)) {
        if ((!hasMode('l') || !isFull()) && 
            (!hasMode('i') || isInvited(user))) {
                _users.push_back(user);
                user->joinChannel(this);
                _invited.erase(user);
        }
    }
}

bool Channel::isInChannel(User *user) const
{
    if (std::find(_users.begin(), _users.end(), user) == _users.end()) {
        return false;
    }
    return true;
}

void Channel::addOperator(User *user)
{
    if (isInChannel(user))
    {
        _operators.insert(user);
    }
}

bool Channel::isOperator(User *user) const
{
    if (_operators.find(user) != _operators.end())
        return true;
    return false;
}

void Channel::removeOperator(User *user)
{
    _operators.erase(user);
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

void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
}

void Channel::removeUserLimit()
{
    _userLimit = 0;
}

bool Channel::isFull() const
{
    if (_userLimit == 0)
        return false;
    return static_cast<int>(_users.size()) >= _userLimit;
}

void Channel::inviteUser(User* user)
{
    if (!isInChannel(user))
        _invited.insert(user);
}

bool Channel::isInvited(User* user)
{
    if (_invited.find(user) != _invited.end())
        return true;
    return false;
}

void Channel::setTopic(const std::string& topic, User *setter)
{
    if (hasMode('t')) {
        if (isInChannel(setter) && isOperator(setter))
            _topic = topic;
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

const std::vector<User *>& Channel::getUsers() const
{
    return (_users);
}

const std::string& Channel::getTopic() const
{
    return (_topic);
}

User *Channel::findUserByNickname(const std::string &nickname) const
{
    int i;
    for(i = 0; i < _users.size(); i++) {
        User* user = _users[i];
        if (user->getNickname() == nickname)
            return user;
    }
    return NULL;
}

void Channel::broadcast(User *sender, const std::string &message)
{
    for(int i = 0; i < _users.size(); i++) {
        if (sender != _users[i])
            // _users[i]->sendMessage(message);
            _users[i]; // a supprimer 
    }
}
