
#include "../server/Server.hpp"

void Server::Mode(std::vector<std::string> params, Client &client)
{
	if (params.size() < 1)
	{
		std::string msg = ":server 461 * MODE :Not enough parameters\r\n";
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

	if (params.size() == 1)
    {
        std::string modes = "+";
        if (newChannel->isInviteOnly())
            modes += "i";
        if (!newChannel->getTopic().empty())
            modes += "t";
        if (!newChannel->getPassword().empty())
            modes += "k";
        if (newChannel->getUserLimit() > 0)
            modes += "l";

        if (modes == "+")
            modes = "+nt";

        std::string reply = ":server 324 " + client.getNickname() + " " + channelName + " " + modes + "\r\n";
        send(client.getFd(), reply.c_str(), reply.size(), 0);
        return;
    }

	if(!newChannel->hasMember(&client))
	{
		std::string msg = ":server 442 " + client.getNickname() + " " + channelName + " :Not on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(!newChannel->hasAdmin(&client))
	{
		std::string msg = "482 " + client.getNickname() + " " + channelName + " :Not channel operator\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string mode = params[1];
	if (mode == "+i")
		newChannel->setInviteOnly(true);
	else if (mode == "-i")
		newChannel->setInviteOnly(false);
	else if (mode == "+t")
		newChannel->setTopicProtected(true);
	else if (mode == "-t")
		newChannel->setTopicProtected(false);
	else if (mode == "+k")
	{
		if (params.size() < 3)
		{
			std::string msg = ":server 461 " + client.getNickname() + " MODE " + channelName + " :Not enough parameters\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		newChannel->setPassword(params[2]);
	}
	else if (mode == "-k")
		newChannel->removePassword();
	else if (mode == "+o")
	{
		if (params.size() < 3)
		{		
			std::string msg = ":server 461 " + client.getNickname() + " MODE " + channelName + " :Not enough parameters\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		Client *admin = getClientByNick(params[2]);
		if (admin && newChannel->hasMember(admin))
			newChannel->addAdmin(admin);
	}
	else if (mode == "-o")
	{
		if (params.size() < 3)
		{		
			std::string msg = ":server 461 " + client.getNickname() + " MODE " + channelName + " :Not enough parameters\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		Client *admin = getClientByNick(params[2]);
		if (admin && newChannel->hasMember(admin) && newChannel->hasAdmin(admin))
			newChannel->removeAdmin(admin);
	}
	else if (mode == "+l" && params.size() > 2)
	{
		if (params[2].empty() || !isdigit(params[2][0]))
		{
			std::string msg = ":server 461 " + client.getNickname() + " MODE " + channelName + " :Not enough parameters\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		newChannel->setUserLimit(atoi(params[2].c_str()));
	}
	else if (mode == "-l")
		newChannel->removeUserLimit();
	else
	{
		std::string msg = ":server 472 " + client.getNickname() + " MODE " + channelName + " :is unknown mode char\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string reply = ":" + client.getNickname() + " MODE " + channelName + " ";
	if ((mode == "+t" || mode == "+k" || mode == "+l") && params.size() > 2)
        reply += " " + params[2];

    reply += "\r\n";
	for (size_t i = 0; i < newChannel->getMembers().size(); i++)
	{
		Client *c = newChannel->getMembers()[i];
		send(c->getFd(), reply.c_str(), reply.size(), 0);
	}
}