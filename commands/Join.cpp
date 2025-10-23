
#include "../server/Server.hpp"

void Server::Join(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = ":server 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	for (size_t i = 0; i < params.size(); ++i)
	{
		std::string channelName = params[i];
		if (channelName[0] != '#')
		{
			std::string msg = ":server 476 " + client.getNickname() + " " + channelName + " :Bad Channel Mask\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			continue;
		}

		Channel *newChannel = getChannelByName(channelName);
		if (!newChannel)
		{
			newChannel = new Channel(channelName);
			channels[channelName] = newChannel;
			newChannel->addAdmin(&client);
		}

		if (newChannel->hasMember(&client))
		{
			std::string msg = ":server 443 " + client.getNickname() + " " + channelName + " :is already on channel\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			continue;
		}

		if (newChannel->getMembers().size() == static_cast<size_t>(newChannel->getUserLimit()))
		{
			std::string msg = ":server 471 " + client.getNickname() + " " + newChannel->getChannelName() + " :Channel is full\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			continue;
		}

		if (newChannel->isInviteOnly() && !newChannel->hasInvited(&client))
		{
			std::string msg = ":server 473 " + client.getNickname() + " " + newChannel->getChannelName() + " :Cannot join channel (+i)\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			continue;
		}

		if(!newChannel->getPassword().empty() && (params.size() < 2 || params[1] != newChannel->getPassword()))
		{
			std::string msg = ":server 475 " + client.getNickname() + " " + newChannel->getChannelName() + " :Cannot join channel (+k)\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			continue;
		}

		newChannel->addClient(&client);
		client.addChannel(newChannel);

		std::string joinMsg = ":" + client.getNickname() + " JOIN " + channelName + "\r\n";

		std::vector<Client*>& members = newChannel->getMembers();
		for (size_t j = 0; j < members.size(); ++j)
			send(members[j]->getFd(), joinMsg.c_str(), joinMsg.size(), 0);

		std::string topic = newChannel->getTopic();
		if (!topic.empty())
		{
			std::string topicMsg = ":server 332 " + client.getNickname() + " " + channelName + " :" + topic + "\r\n";
			send(client.getFd(), topicMsg.c_str(), topicMsg.size(), 0);
		}
		else
		{
			std::string noTopicMsg = ":server 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
			send(client.getFd(), noTopicMsg.c_str(), noTopicMsg.size(), 0);
		}

		std::string namesMsg = ":server 353 " + client.getNickname() + " = " + channelName + " :";
		for (size_t j = 0; j < members.size(); ++j)
		{
			if (newChannel->hasAdmin(members[j]))
				namesMsg += "@";
			namesMsg += members[j]->getNickname() + " ";
		}
		
		namesMsg += "\r\n";
		send(client.getFd(), namesMsg.c_str(), namesMsg.size(), 0);
		std::string endNamesMsg = ":server 366 " + client.getNickname() + " " + channelName + " :End of NAMES list\r\n";
		send(client.getFd(), endNamesMsg.c_str(), endNamesMsg.size(), 0);
	}
}