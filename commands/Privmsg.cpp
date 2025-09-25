// mesaj göndermek için

#include "../server/Server.hpp"

void Server::Privmsg(std::vector<std::string> params, Client &client)
{
	if (params.size() < 2) 
	{
		std::string msg = "461 PRIVMSG :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string target = params[0];
	std::string message = params[1];

	if (target[0] == '#')
	{
		Client* targetClient = getClientByNick(target);
		if (targetClient)
		{
			std::string msg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
			send(targetClient->getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		std::string msg = "401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	
	std::map<std::string, Channel*>::iterator ch = channels.find(target);
	if (ch == channels.end())
	{
		std::string msg = "403 " + client.getNickname() + " " + target + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	Channel *channel = ch->second;
    if (!channel->hasMember(&client))
	{
        std::string msg = "404 " + client.getNickname() + " " + target + " :Cannot send to channel\r\n";
        send(client.getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
	std::string msg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
	channel->broadcast(msg, client.getFd());
}