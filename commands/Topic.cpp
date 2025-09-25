//kanal başlığını değitirir

#include "../server/Server.hpp"

void Server::Topic(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = "461 " + client.getNickname() + "TOPIC :Not enough parameters\r\n";		send(client.getFd(), msg.c_str(), msg.size(), 0);
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

	if(!newChannel->hasMember(&client))
	{
		std::string msg = "442 " + client.getNickname() + " " + channelName + " :Not on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(!newChannel->hasAdmin(&client))
	{
		std::string msg = "482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (params.size() == 1)
	{
		std::string currentTopic = newChannel->getTopic();
		if (currentTopic.empty())
		{
		    std::string msg = "331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
		}
		else
		{
		    std::string msg = "332 " + client.getNickname() + " " + channelName + " :" + currentTopic + "\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
		}
		return;
    }

	newChannel->setTopic(params[1]);
    std::string notice = ":" + client.getNickname() + " TOPIC " + channelName + " :" + params[1] + "\r\n";
	send(client.getFd(), notice.c_str(), notice.size(), 0);
}