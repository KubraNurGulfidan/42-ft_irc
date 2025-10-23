
#include "../server/Server.hpp"

void Server::Kick(std::vector<std::string> params, Client &client)
{
	if (params.size() < 2)
	{
		std::string msg = ":server 461 " + client.getNickname() + " KICK :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string channelName = params[0];
	Channel *newChannel = getChannelByName(channelName);
	if (!newChannel)
	{
		std::string msg = ":server 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(!newChannel->hasMember(&client))
	{
		std::string msg = ":server 442 " + client.getNickname() + " " + channelName + " :Not on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (!newChannel->hasAdmin(&client))
	{
		std::string msg = ":server 482 " + client.getNickname() + " " + params[1] + " :You're not channel operator\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	Client* target = getClientByNick(params[1]);
	if (!target)
	{
		std::string msg = ":server 401 " + client.getNickname() + " " + params[0] + " :No such nick/channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (!newChannel->hasMember(target))
	{
        std::string msg = ":server 441 " + client.getNickname() + " " + params[1] + " " + channelName + " :They aren't on that channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	newChannel->removeClient(target);

    std::string msg = ":" + client.getNickname() + " KICK " + channelName + " " + params[1] + "\r\n";
    send(target->getFd(), msg.c_str(), msg.size(), 0);

    std::vector<Client*>& members = newChannel->getMembers();
    for (size_t i = 0; i < members.size(); ++i)
        send(members[i]->getFd(), msg.c_str(), msg.size(), 0);
}