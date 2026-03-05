/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pribolzi <pribolzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 14:44:24 by pribolzi          #+#    #+#             */
/*   Updated: 2026/03/05 16:48:22 by pribolzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::kick_com(std::string message, Client* sender) {
	if (isInChannel(findClientByNickname(message)))
		removeClient(findClientByNickname(message));
	//c'est tout ????
}

void Channel::invite_com(std::string message, Client* sender) {
	if (!isInvited(findClientByNickname(message)))
		if (!isFull())
			inviteClient(findClientByNickname(message));
	//c'est tout ?????
}

void Channel::topic_com(std::string message, Client* sender) {
	if (_topic != message)
		setTopic(message, sender);
	//je crois bien que c'est tout...
}

//c'est vraiment pas tout

void Channel::mode_com(std::string message, Client* sender) {
	char mode = message[0];
	std::string arg = "";
	if (std::strlen(message.c_str()) > 1)
		arg = message.substr(1, std::strlen(message.c_str()));
	//rajouter une fonction UpdateMode pour enlever la repetition des if else en boucle
	//ex : updateMode('i') -> verifie si i est actif, si actif le desactive et remove, sinon l'ajoute et l'active
	switch(mode) {
		case 'i' :
			if (hasMode(mode)) 
				removeMode(mode);
			else
				addMode(mode);
		case 't' :
			if (hasMode(mode))
				removeMode(mode);
			else
				addMode(mode);
		case 'k':
			if (hasMode(mode))
				removeKey(arg);
			else {
				addMode(mode);
				setKey(arg);
			}
		case 'o': 
		if (isInChannel(findClientByNickname(arg))) {
			if (isOperator(findClientByNickname(arg)))
				removeOperator(findClientByNickname(arg));
			else
				addOperator(findClientByNickname(arg));
		}
		case 'l':
			if (hasMode(mode)) {
				removeMode(mode);
				removeClientLimit();
			}
			else {
				if (arg != "") {
					addMode(mode);
					setClientLimit(std::stoi(arg));
				}
		}
	}
}

void Channel::parse_commands(std::string message, Client* sender) {
	void (Channel::*ptr[4])(std::string msg, Client* sender) = {&Channel::kick_com , &Channel::invite_com, &Channel::topic_com, &Channel::mode_com};
	std::string commands[4] = {"KICK", "INVITE", "TOPIC", "MODE"};
	std::string cmd = message.substr(0, message.find_first_of(" "));
	for (int i = 0; i < 4; ++i) {
		if (commands[i].compare(0, std::strlen(commands[i].c_str()), message) == 0) {
			std::string arg = message.substr(message.find_first_of(" ") + 1, std::strlen(message.c_str()));
			(this->*ptr[i])(arg, sender);
		}
	}
}