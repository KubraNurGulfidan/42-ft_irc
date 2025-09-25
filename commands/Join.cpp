// bir kanala katılmak için

#include "../server/Server.hpp"

void Server::Join(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = "461 " + client.getNickname() + " JOIN :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string channelName = params[0];
	Channel *newChannel = getChannelByName(channelName);
	if (!newChannel)
	{
		newChannel = new Channel(channelName);
		channels[channelName] = newChannel;
	}

	if (newChannel->hasMember(&client))
	{
		std::string msg = "443 " + client.getNickname() + " " + channelName + " :is already on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
        return;
	}

	if (newChannel->getMembers().size() >= static_cast<size_t>(newChannel->getUserLimit()))
	{
		std::string msg = "471 " + client.getNickname() + " " + newChannel->getChannelName() + " :Channel is full\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	// if (newChannel->isInviteOnly() && !newChannel->hasAdmin(&client))
	// {
	// 	std::string msg = "473 " + client.getNickname() + " " + newChannel->getChannelName() + " :Cannot join channel (+i)\r\n";
	// 	send(client.getFd(), msg.c_str(), msg.size(), 0);
	// 	std::cout << msg;
	// 	return;
	// }

	if (newChannel->isInviteOnly() && !newChannel->hasInvited(&client))
	{
		std::string msg = "473 " + client.getNickname() + " " + newChannel->getChannelName() + " :Cannot join channel (+i)\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(!newChannel->getPassword().empty() && (params.size() < 2 || params[1] != newChannel->getPassword()))
	{
		std::string msg = "475 " + client.getNickname() + " " + newChannel->getChannelName() + " :Cannot join channel (+k)\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	newChannel->addClient(&client);
	client.addChannel(newChannel);
}