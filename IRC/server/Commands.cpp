#include "Server.hpp"

void sendMsg(const int& fd, const std::string& msg) {
    std::string line = msg + "\r\n";
    send(fd, line.c_str(), line.size(), 0);
}

static std::string prefix(const Client* c)
{
    return ":" + c->getNickname() + "!" + c->getClientname() + "@localhost";
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

void Server::ping_com(std::vector<std::string> args, Client* sender)
{
    std::string token = args.empty() ? "server" : args[0];
    sendMsg(sender->getFd(), ":server PONG server :" + token);
}

//rajouter des consts a toute mes variables que j'utilise uniquement dans mes commandes (notamment toute mes std::string> target/channame etc)

void Server::join_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}
	if (args.empty()) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " JOIN :Not enouh parameters");
		return;
	}
	const std::vector<std::string> chans = split(args[0], ',');
	std::vector<std::string> keys;
	if (args.size() >= 2)
		keys = split(args[1], ',');
	for (size_t i = 0; i < chans.size(); ++i) {
		const std::string channame = chans[i];
		const std::string key = (i  < keys.size()) ? keys[i] : "";
		if (channame.empty() || channame[0] != '#') {
			sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + channame + " :No such channel");
			continue;
		}
		Channel* chan = createChannel(channame);
		if (chan->isInChannel(sender))
			continue;
		if (chan->hasMode('k') && !chan->checkKey(key)) {
			sendMsg(sender->getFd(), ":server 475 " + sender->getNickname() + " " + channame + " :Cannot join channel (+k)");
			continue;
		}
		if (chan->hasMode('i') && !chan->isInvited(sender)) {
			sendMsg(sender->getFd(), ":server 473 " + sender->getNickname() + " " + channame + " :Cannot join channel (+i)");
			continue;
		}
		if (chan->hasMode('l') && chan->isFull()) {
			sendMsg(sender->getFd(), ":server 471 " + sender->getNickname() + " " + channame + " :Cannot join channel (+l)");
			continue;
		}
		chan->addClient(sender);
		if (chan->getClients().size() == 1)
			chan->addOperator(sender);
		const std::string joinmsg = prefix(sender) + " JOIN :" + channame;
		sendMsg(sender->getFd(), joinmsg);
		chan->broadcast(sender, joinmsg);
		if (!chan->getTopic().empty())
			sendMsg(sender->getFd(), ":server 332 " + sender->getNickname() + " " + channame + " :" + chan->getTopic());
		else
			sendMsg(sender->getFd(), ":server 331" + sender->getNickname() + " " + channame + " :No topic is set");
		std::string names;
		const std::vector<Client *> members = chan->getClients();
		for (size_t j = 0; j < members.size(); ++j) {
			if (chan->isOperator(members[j]))
				names += "@";
			names += members[j]->getNickname();
			if (j + 1 < members.size())
				names += " ";
		}
		sendMsg(sender->getFd(), ":server 353 " + sender->getNickname() + " = " + channame + " :" + names);
		sendMsg(sender->getFd(), ":server 366 " + sender->getNickname() + " " + channame + " :End of /NAMES list");
	}
}

void Server::mode_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}
	if (args.empty()) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " MODE :Not enough parameters");
		return;
	}
	const std::string channame = args[0];
	Channel* chan = getChannelByName(channame);
	if (!chan) {
		sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + channame + " :No such channel");
		return;
	}
	if (args.size() == 1) {
		sendMsg(sender->getFd(), ":server 324 " + sender->getNickname() + " " + channame + " " + chan->getModes());
		return;
	}
	if (!chan->isOperator(sender)) { 
		sendMsg(sender->getFd(), ":server 482 " + sender->getNickname() + " " + channame + " :You're not channel operator");
		return;
	}
	std::string modes = args[1];
	bool adding = true;
	size_t argidx = 2;
	for (size_t i = 0; i < modes.size(); ++i) {
		char c = modes[i];
		if (c == '+')
			adding = true;
		if (c == '-')
			adding = false;
		switch (c) {
			case 'i':
				adding ? chan->addMode(c) : chan->removeMode(c);
				break;
			case 't':
				adding ? chan->addMode(c) : chan->removeMode(c);
				break;
			case 'k':
				if (adding) {
					if (argidx >= args.size()) {
						sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " MODE :Not enough parameters");
						break;
					}
					chan->addMode('k');
					chan->setKey(args[argidx]);
					argidx++;
				}
				else {
					chan->removeMode('k');
					//chan->removeKey(chan->getKey());
				}
				break;
			case 'o': {
				if (argidx >= args.size()) {
					sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " MODE :Not enough parameters");
					break;
				}
				const std::string targetname = args[argidx];
				argidx++;
				Client* target = chan->findClientByNickname(targetname);
				if (!target) {
					sendMsg(sender->getFd(), ":server 441 " + sender->getNickname() + " " + targetname + " " + channame + " :They aren't on that channel");
					return ;
				}
				adding ? chan->addOperator(target) : chan->removeOperator(target);
				break;
			}
			case 'l':
				if (adding) {
					if (argidx >= args.size()) {
						sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " MODE :Not enough parameters");
						break;
					}
					chan->addMode(c);
					chan->setClientLimit(std::atoi(args[argidx].c_str()));
					argidx++;
				}
				else {
					chan->removeMode('l');
					chan->removeClientLimit();
				}
				break;
			default:
				sendMsg(sender->getFd(), ":server 472 " + sender->getNickname() + " " + c + " :is unknown mode char");
				break;
		}
	}
	const std::string modemsg = prefix(sender) + " MODE " + channame + " " + modes;
	chan->broadcast(sender, modemsg);
	sendMsg(sender->getFd(), modemsg);
}
void Server::topic_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}
	if (args.empty()) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " TOPIC :Not enough parameters");
		return;
	}
	const std::string channame = args[0];
	Channel* chan = getChannelByName(channame);
	if (!chan) {
		sendMsg(sender->getFd(), "server 403 " + sender->getNickname() +  " " + channame + " :No such channel");
		return;
	}
	if (!chan->isInChannel(sender)) {
		sendMsg(sender->getFd(), ":server 442 " + sender->getNickname() + " " + channame + ":You're not on that channel");
		return;
	}
	if (args.size() == 1) {
		if (chan->getTopic().empty())
			sendMsg(sender->getFd(), ":server 331 " + sender->getNickname() + " " + channame + " :No topic is set");
		else
			sendMsg(sender->getFd(), ":server 332 " + sender->getNickname() + " " + channame + " :" + chan->getTopic());
		return;
	}
	if (chan->hasMode('t') && !chan->isOperator(sender)) {
		sendMsg(sender->getFd(), ":server 482 " + sender->getNickname() + " " + channame + " :You're not channel operator");
		return;
	}
	chan->setTopic(args[1], sender);
	const std::string topicmsg = prefix(sender) + " TOPIC " + channame + " :" + args[1];
	chan->broadcast(sender, topicmsg);
	sendMsg(sender->getFd(), topicmsg);
}

void Server::kick_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}
	if (args.size() < 2) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " KICK :Not enough parameters");
		return ;
	}
	const std::string channame = args[0];
	const std::string targetname = args[1];
	const std::string reason = (args.size() >= 3) ? args[2] : sender->getNickname();
	Channel* chan = getChannelByName(channame);
	if (!chan) {
		sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + channame + " :No such channel");
		return ;
	}
	if (!chan->isInChannel(sender)) {
		sendMsg(sender->getFd(), ":server 442 " + sender->getNickname() + " " + args[0] + " :You're not on that channel");
		return ;
	}
	if (!chan->isOperator(sender)) {
		sendMsg(sender->getFd(), ":server 482 " + sender->getNickname() + " " + channame + " :You're not channel operator");
		return;
	}
	Client* target = chan->findClientByNickname(targetname);
	if (!target) {
		sendMsg(sender->getFd(), ":server 441 " + sender->getNickname() + " " + targetname + " " + channame + " :They aren't on that channel");
		return ;
	}
	const std::string kickmsg = prefix(sender) + " KICK " + channame + " " + targetname + " :" + reason;
	sendMsg(target->getFd(), kickmsg);
	chan->removeClient(target);
	chan->broadcast(sender, kickmsg);
}

void Server::invite_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}
	if (args.size() < 2) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " INVITE :Not enough parameters");
		return ;
	}
	const std::string targetname = args[0];
	const std::string channame = args[1];
	Channel* chan = getChannelByName(channame);
	if (!chan) {
		sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + channame + " :No such channel");
		return ;
	}
	if (!chan->isInChannel(sender)) {
		sendMsg(sender->getFd(), ":server 442 " + sender->getNickname() + " " + args[0] + " :You're not on that channel");
		return ;
	}
	if (chan->hasMode('i') && !chan->isOperator(sender)) {
		sendMsg(sender->getFd(), ":server 482 " + sender->getNickname() + " " + channame + " :You're not channel operator");
		return;
	}
	Client* target = chan->findClientByNickname(targetname);
	if (!target) {
		sendMsg(sender->getFd(), ":server 401 " + sender->getNickname() + " " + targetname + " :No such nick");
		return;
	}
	if (chan->isInChannel(target)) {
		sendMsg(sender->getFd(), ":server 443 " + sender->getNickname() + " " + targetname + " " + channame + " :is already on channel");
		return ;
	}
	chan->inviteClient(target);
	sendMsg(sender->getFd(), ":server 341 " + sender->getNickname() + " " + targetname + " " + channame);
	sendMsg(target->getFd(), prefix(sender) + " INVITE " + targetname + " :" + channame);
}

void Server::pass_com(std::vector<std::string> args, Client* sender) {
	if (sender->hasNick() || sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 462 " + sender->getNickname() + " :You may not reregister");
		return ;
	}
	if (args[0].empty()) {
		sendMsg(sender->getFd(), ":server 461 * PASS :Not enough parameters");
		return ;
	}
	if (args[0] != _password) {
		sendMsg(sender->getFd(), ":server 464 * :Password incorrect");
		return;
	}
	sender->setHasPass(true);
}

void Server::user_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasPass()) {
        sendMsg(sender->getFd(), ":server 464 * :Password required");
        return;
	}
    if (sender->hasClient()) {
        sendMsg(sender->getFd(), ":server 462 " + sender->getNickname() + " :You may not reregister");
        return;
    }
	if (args.size() < 4) {
		sendMsg(sender->getFd(), ":server 461 * USER :Not enough parameters");
        return;
	}
	const std::string username = args[0];
	const std::string realname = args[3];
	sender->setNickname(username);
	sender->setClientname(realname);
	sender->setHasClient(true);
}

void Server::privmsg_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
        sendMsg(sender->getFd(), ":server 451 * :You have not registered");
        return;
    }
	if (args.size() < 2 || args[0].empty() || args[1].empty()) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " PRIVMSG :Not enough parameters");
        return;
	}
	const std::string target = args[0];
	const std::string msg = args[1];
	if (target[0] == '#') {
		Channel* chan = getChannelByName(target);
		if (!chan)       {
            sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + target + " :No such channel");
            return;
        }
		if (!chan->isInChannel(sender)) {
            sendMsg(sender->getFd(), ":server 404 " + sender->getNickname() + " " + target + " :Cannot send to channel");
            return;
        }
		const std::string finalmsg = prefix(sender) + " PRIVMSG " + target + " :" + msg;
		chan->broadcast(sender, msg);
	}
	else { 
		Client* dest = getClientByName(target);
		if (!dest) {
			sendMsg(sender->getFd(), ":server 401 " + sender->getNickname() + " " + target + " :No such nick");
			return ;
		}
		const std::string finalmsg = prefix(sender) + " PRIVMSG " + target + " :" + msg;
		sendMsg(dest->getFd(), finalmsg);
	}
}

void Server::nick_com(std::vector<std::string> args, Client* sender) {
    if (!sender->hasPass()) {
        sendMsg(sender->getFd(), ":server 464 * :Password required");
        return;
	}
	if (args.empty()) {
		if (sender->hasNick())
			sendMsg(sender->getFd(), sender->getNickname());
		//ligne du dessus pas totalement finit
		sendMsg(sender->getFd(), ":server 461 * :No nickname given");
		return ;
	}
	const std::string newNick = args[0];
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getNickname() == newNick && _clients[i]->getFd() != sender->getFd()) {
			sendMsg(sender->getFd(), ":server433 * " + newNick + " :Nickname is already in use");
			return ;
		}
	}
	sender->setNickname(newNick);
	sender->setHasNick(true);
}

void Server::part_com(std::vector<std::string> args, Client* sender) {
	if (!sender->hasClient()) {
		sendMsg(sender->getFd(), ":server 451 * :You have not registered");
		return;
	}if (args[0].empty()) {
		sendMsg(sender->getFd(), ":server 461 " + sender->getNickname() + " PART :Not enough parameters");
	}
	const std::vector<std::string> channels = split(args[0], ',');
	const std::string reason = (args.size() > 1 ) ? args[1] : sender->getNickname();
	for (size_t i = 0; i < channels.size(); ++i) {
		const std::string channame = channels[i];
		Channel* chan = getChannelByName(args[0]);
		if (!chan) {
			sendMsg(sender->getFd(), ":server 403 " + sender->getNickname() + " " + args[0] + " :No such channel");
			return ;
		}
		if (!chan->isInChannel(sender)) { 
			sendMsg(sender->getFd(), ":server 442 " + sender->getNickname() + " " + args[0] + " :You're not on that channel");
			return ;
		}
		const std::string partmsg = prefix(sender) + " PART " + channame + " :" + reason;
		sendMsg(sender->getFd(), partmsg);
		chan->broadcast(sender, partmsg);
		chan->removeClient(sender);
	}
}

void Server::quit_com(std::vector<std::string> args, Client* sender) {
	const std::string reason = args.empty() ? "Client quit" : args[0];
	const std::string quitMsg = prefix(sender) + "QUIT :" + reason;

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
	size_t last = message.find_last_not_of("\r\n");
	if (last != std::string::npos)
    	message = message.substr(0, last + 1);
	std::vector<std::string> msg = split(message, ' ');
    std::transform(msg[0].begin(), msg[0].end(), msg[0].begin(), ::toupper);
	std::string cmd = msg[0];
	msg.erase(msg.begin());
	std::map<std::string, void (Server::*)(std::vector<std::string>, Client*)>::iterator it;
	it = _commands.find(cmd);
	if (it != _commands.end())
		(this->*(it->second))(msg, sender);
}

std::vector<std::string> split(std::string s, char c) {
    size_t pos_start = 0, pos_end;
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(c, pos_start)) != std::string::npos) {

        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = s.find_first_not_of(' ', pos_end);
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
	//solution du dessous faites juste pour parse_commands, peut amener des erreurs dans les commandes ou j'utilise split, a tester et modifier
	if (res.size() == 1)
		res.push_back("");
    return res;
}