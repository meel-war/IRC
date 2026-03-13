#pragma once

#include <string>
#include <iostream>
#include <set>
#include <cstring>

class Channel;

class Client {

    private:
        std::string _nickname;
        std::string _clientname;
        bool _has_pass;
        bool _has_nick;
        bool _has_client;
        std::set<Channel*> _joinedChannels;
        int _client_fd;
        std::string _buffer;

    public:
        Client(int fd);
        void joinChannel(Channel* channel);
        const std::string& getNickname() const;
        const std::string& getClientname() const;
        const int& getFd() const;
        void setNickname(std::string& nickname);
        void setClientname(std::string& clientname);
        bool hasPass() const;
        bool hasNick() const;
        bool hasClient() const;
        void setHasPass(bool pass);
        void setHasNick(bool nick);
        void setHasClient(bool client);
        void appendBuffer(const std::string &new_buffer);
        std::string& getBuffer();
};
