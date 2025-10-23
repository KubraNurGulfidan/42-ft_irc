
#include "../server/Server.hpp"

void Server::Notice(std::vector<std::string> params, Client &client)
{
	if (params.size() < 2) 
		return;

	std::string target = params[0];
	std::string message = params[1];

	if (target[0] != '#')
	{
		Client* targetClient = getClientByNick(target);
		if (targetClient)
		{
			std::string msg = ":" + client.getPrefix() + " NOTICE " + target + " :" + message + "\r\n";
			send(targetClient->getFd(), msg.c_str(), msg.size(), 0);
		}
		return;
	}
	
	std::map<std::string, Channel*>::iterator ch = channels.find(target);
	if (ch != channels.end())
	{
		Channel *channel = ch->second;
		std::string msg = ":" + client.getPrefix() + " NOTICE " + target + " :" + message + "\r\n";
		channel->broadcast(msg, client.getFd());
	}
	return;
}