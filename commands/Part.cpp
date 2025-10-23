
#include "../server/Server.hpp"

void Server::Part(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = ":server 461 " + client.getNickname() + " PART :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string channelName = params[0];
	Channel *channel = getChannelByName(channelName);
	if (!channel)
	{
		std::string msg = ":server 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(!channel->hasMember(&client))
	{
		std::string msg = ":server 442 " + client.getNickname() + " " + channelName + " :Not on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::vector<Client*> members = channel->getMembers();
	std::string partMsg = ":" + client.getNickname() + " PART " + channelName + "\r\n";

	for (size_t i = 0; i < members.size(); ++i)
		send(members[i]->getFd(), partMsg.c_str(), partMsg.size(), 0);

	channel->removeClient(&client);
	client.removeChannel(channel);

	if (channel->getMembers().empty())
	{
		delete channel;
		channels.erase(channelName);
	}
}