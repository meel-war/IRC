#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "../client/Client.hpp"

class Channel {

    private:
        std::string _name;
        std::string _topic;
        std::string _key;
        int _clientLimit;
        std::vector<Client *> _clients;
        std::set<Client*> _operators;
        std::set<char> _modes;
        std::set<Client *> _invited;

    public:
        Channel(std::string name);
        void removeClient(Client* client);
        void addClient(Client* client);
        bool isInChannel(Client* client) const;
        void addOperator(Client* client);
        bool isOperator(Client* client) const;
        void removeOperator(Client *client);
        void addMode(char mode);
        bool hasMode(char mode) const;
        void removeMode(char mode);
        void setKey(const std::string& key);
        void removeKey(const std::string &key);
        bool checkKey(const std::string& input) const;
        void setClientLimit(int limit);
        void removeClientLimit();
        bool isFull() const;
        void inviteClient(Client* client);
        bool isInvited(Client *client);
        void setTopic(const std::string& topic, Client* setter);
        std::string getModes() const;
        std::string getName() const;
        const std::vector<Client*>& getClients() const;
        const std::string& getTopic() const;
        Client* findClientByNickname(const std::string& nickname) const;
        void broadcast(Client* sender, const std::string& message);
        void updateMode(char c);
		const std::string getKey() const;
};
