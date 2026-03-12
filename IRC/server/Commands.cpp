#include "Server.hpp"

void sendMsg(const int& fd, const std::string& msg) {
    std::string line = msg + "\r\n";
    send(fd, line.c_str(), line.size(), 0);
}

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

void Server::nick_com(std::vector<std::string> args, Client* sender) {
    if (!sender->hasPass()) {
        sendMsg(sender->getFd(), ":server 464 * :Password required");
        return;
	}
	if (args.empty()) {
		sendMsg(sender->getFd(), ":server 461 * :No nickname given");
		return ;
	}
	std::string newNick = args[0];
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getNickname() == newNick && _clients[i]->getFd() != sender->getFd()) {
			sendMsg(sender->getFd(), ":server433 * " + newNick + " :Nickname is already in use");
			return ;
		}
	}
	sender->setNickname(newNick);
	//sender->setHasNick(true);
}

void Server::part_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}if (args[0].empty() || args[1].empty()) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " PART :Not enough parameters");
	}
	//enlever le # du nom du channel ?
	std::string reason = (args.size() > 1 ) ? args[1] : sender->getNickname();
	if (std::find(_channels.begin(), _channels.end(), args[0]) != _channels.end()) {
		sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + args[0] + " :No such channel");
		return ;
	}
	Channel& currchan = _channels[&sender];
	if (!currchan.isInChannel(sender)) { 
		sendMsg(sender->getFd(), ":server 442 " + sender->getNickname() + " " + args[0] + " :You4re not on that channel");
		return ;
	}
	std::string part = sender->getNickname() + " PART " + args[0] + " :" + reason;
	sendMsg(sender->getFd(), part);
	currchan.broadcast(sender, part);
	currchan.removeClient(sender);
}

void Server::quit_com(std::vector<std::string> args, Client* sender) {
	std::string reason = args.empty() ? "Client quit" : args[0];
	std::string quitMsg = sender->getNickname() + "QUIT :" + reason;

	std::vector<Channel*>::iterator it = _channels.begin();
	for(; it != _channels.end(); ++it) {
		Channel* chan = *it;
		if (chan->isInChannel(sender)) {
			chan->broadcast(sender, quitMsg);
			chan->removeClient(sender);
		}
	}
}

void Server::parse_commands(std::string message, Client* sender) {
	// /r /n a la fin du msg = msg complet, sinon garder en memoire et lire la suite puir concatener
    std::vector<std::string> msg = split(message, " ");
    std::transform(msg[0].begin(), msg[0].end(), msg[0].begin(), ::toupper);
	std::string cmd = msg[0];
	msg.erase(msg.begin());
    std::map<std::string, void(Server::*)(std::vector<std::string>, Client*)>::iterator it = _commands.find(cmd);
    if (it != _commands.end())
        (this->*(it->second))(msg, sender);
}
//si il n'y a pas d'argument a voir ce qu'est msg apres le erase, peut etre devoir changer la methode

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