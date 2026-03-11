#include "Server.hpp"
#include <sstream>
#include <algorithm>



void Server::init_commands()
{
    _commands["PASS"]    = &Server::pass_com;
    _commands["NICK"]    = &Server::nick_com;
    _commands["USER"]    = &Server::user_com;
    _commands["JOIN"]    = &Server::join_com;
    _commands["PART"]    = &Server::part_com;
    _commands["PRIVMSG"] = &Server::privmsg_com;
    _commands["PING"]    = &Server::ping_com;
    _commands["QUIT"]    = &Server::quit_com;
    _commands["INVITE"]  = &Server::invite_com;
    _commands["KICK"]    = &Server::kick_com;
    _commands["TOPIC"]   = &Server::topic_com;
    _commands["MODE"]    = &Server::mode_com;
}

void Server::parse_commands(std::string message, Client& sender) {
    std::vector<std::string> msg = split(message, " ");
    _commands[msg[0]]->second;
}

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}