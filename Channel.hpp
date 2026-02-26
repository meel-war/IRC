#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "User.hpp"

class Channel {

    private:
        std::string _name;
        std::string _topic;
        std::string _key;
        int _userLimit;
        std::vector<User *> _users;
        // set c'est comme vector mais ne stocke pas les doublons et ignore l’element si présent
        std::set<User*> _operators;
        std::set<char> _modes;
        std::set<User *> _invited;

    public:
        void removeUser(User* user);
        void addUser(User* user);
        bool isInChannel(User* user) const;
        void addOperator(User* user);
        bool isOperator(User* user) const;
        void removeOperator(User *user);
        void addMode(char mode);
        bool hasMode(char mode) const;
        void removeMode(char mode);
        void setKey(const std::string& key);
        void removeKey();
        bool checkKey(const std::string& input) const;
        void setUserLimit(int limit);
        void removeUserLimit();
        bool isFull() const;
        void inviteUser(User* user);
        bool isInvited(User *user);
        void setTopic(const std::string& topic, User* setter);
        std::string getModes() const;
        const std::vector<User*>& getUsers() const;
        const std::string& getTopic() const;
        User* findUserByNickname(const std::string& nickname) const;
        void broadcast(User* sender, const std::string& message);

};