#pragma once

#include <string>
#include <iostream>
#include <set>

class Channel;

class User {

    private:
        std::string _nickname;
        std::string _username;
        bool _has_pass;
        bool _has_nick;
        bool _has_user;
        std::set<Channel*> _joinedChannels;
        int _client_fd;

    public:
        User(int fd);
        void joinChannel(Channel* channel);
        const std::string& getNickname() const;
        const std::string& getUsername() const;
        void setNickname(std::string& nickname);
        void setUsername(std::string& username);
        bool hasPass() const;
        bool hasNick() const;
        bool hasUser() const;
};