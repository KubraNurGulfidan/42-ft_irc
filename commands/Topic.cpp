//kanal başlığını değitirir
// BİTTİ

#include "../server/Server.hpp"

void Server::Topic(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = ":server 461 * TOPIC :Not enough parameters\r\n";		send(client.getFd(), msg.c_str(), msg.size(), 0);
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

	if(!channel->hasAdmin(&client))
	{
		std::string msg = ":server 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (params.size() == 1)
	{
		std::string currentTopic = channel->getTopic();
		if (currentTopic.empty())
		{
			std::string msg = ":server 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
		}
		else
		{
			std::string msg = ":server 332 " + client.getNickname() + " " + channelName + " :" + currentTopic + "\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
		}
		return;
	}

	channel->setTopic(params[1]);
	std::string notice = ":" + client.getNickname() + " TOPIC " + channelName + " :" + params[1] + "\r\n";
	for (size_t i = 0; i < channel->getMembers().size(); ++i)
    {
        Client *c = channel->getMembers()[i];
        send(c->getFd(), notice.c_str(), notice.size(), 0);
    }
}