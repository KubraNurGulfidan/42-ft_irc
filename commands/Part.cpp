// kanaldan ayrılmayı sağlar

#include "../server/Server.hpp"

void Server::Part(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = "461 " + client.getNickname() + " PART :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string channelName = params[0];
	Channel *newChannel = getChannelByName(channelName);
	if (!newChannel)
	{
		std::string msg = "403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(newChannel->hasMember(&client))
	{
		std::string msg = "442 " + client.getNickname() + " " + channelName + " :Not on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	newChannel->removeClient(&client);
	client.removeChannel(newChannel);

	if (newChannel->getMembers().empty())
	{
        channels.erase(channelName);
        delete newChannel;
    }
}