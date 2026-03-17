#pragma once

#include <string>
#include <iostream>
#include <set>
#include <cstring>

class Channel;

class Client {

    private:
		int _client_fd;
        std::string _nickname;
        std::string _clientname;
		std::string _realname;
        bool _has_pass;
        bool _has_nick;
        bool _has_client;
		bool _has_realname;
        std::set<Channel*> _joinedChannels;
        std::string _buffer;

    public:
        Client(int fd);
        void joinChannel(Channel* channel);
        const std::string& getNickname() const;
        const std::string& getClientname() const;
        const int& getFd() const;
        void setNickname(const std::string& nickname);
        void setClientname(const std::string& clientname);
        bool hasPass() const;
        bool hasNick() const;
        bool hasClient() const;
		bool hasRealName() const;
        void setHasPass(bool pass);
        void setHasNick(bool nick);
        void setHasClient(bool client);
        void appendBuffer(const std::string &new_buffer);
        std::string& getBuffer();
		std::string& getRealName();
		void setHasRealName(bool pass);
		void setRealName(const std::string realname);
};
