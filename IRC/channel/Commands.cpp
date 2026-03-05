/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pribolzi <pribolzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 14:44:24 by pribolzi          #+#    #+#             */
/*   Updated: 2026/03/05 15:33:53 by pribolzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::kick_com(std::string message) {
	if (isInChannel(findClientByNickname(message)))
		removeClient(findClientByNickname(message));
	//c'est tout ????
}

void Channel::invite_com(std::string message) {
	if (!isInvited(findClientByNickname(message)))
		inviteClient(findClientByNickname(message));
	//c'est tout ?????
}

void Channel::topic_com(std::string message, Client* sender) {
	if (_topic != message)
		setTopic(message, sender);
	//je crois bien que c'est tout...
}

void Channel::mode_com(std::string message) {
	
}

void Channel::parse_commands(std::string message, Client* sender) {
	void (Channel::*ptr[4])(std::string msg) = {&Channel::kick_com , &Channel::invite_com, &Channel::topic_com, &Channel::mode_com};
	std::string commands[4] = {"KICK", "INVITE", "TOPIC", "MODE"};
	std::string cmd = message.substr(0, message.find_first_of(" "));
	for (int i = 0; i < 4; ++i) {
		if (commands[i].compare(0, std::strlen(commands[i].c_str()), message) == 0) {
			std::string arg = message.substr(message.find_first_of(" ") + 1, std::strlen(message.c_str()));
			(this->*ptr[i])(arg);
		}
	}
}