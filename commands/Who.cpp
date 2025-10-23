
#include "../server/Server.hpp"

std::string Server::getMaskedHostname(const std::string& hostname) const
{
	if (hostname.length() > 8)
		return "*." + hostname.substr(hostname.find('.') + 1);
	return hostname;
}

std::string Server::getUserPrefix(Client* user, Channel* channel) const
{
	if (!channel)
		return "";
	
	if (channel->hasAdmin(user))
		return "@";
	
	return "";
}

void Server::Who(std::vector<std::string> params, Client &client)
{
	if (params.empty())
	{
		for (size_t i = 0; i < clients.size(); ++i)
		{
			Client* c = clients[i];
			std::string status = c->getAway() ? "G" : "H";
			if (c->getIsOperator())
				status += "*";
			
			std::string maskedHost = getMaskedHostname(c->getHostname());
			std::string msg = ":server 352 " + client.getNickname() + " * " 
							+ c->getUsername() + " " 
							+ maskedHost + " "
							+ "server "
							+ c->getNickname() + " " 
							+ status + " :0 "
							+ c->getRealname() + "\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
		}
	}
	else
	{
		std::string channelName = params[0];
		Channel* ch = getChannelByName(channelName);
		if (!ch)
		{
			std::string msg = ":server 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}

		std::vector<Client*>& members = ch->getMembers();
		for (size_t i = 0; i < members.size(); ++i)
		{
			Client* c = members[i];
			std::string prefix = getUserPrefix(c, ch);
			std::string status = c->getAway() ? "G" : "H";
			if (c->getIsOperator())
				status += "*";
			
			std::string maskedHost = getMaskedHostname(c->getHostname());
			std::string msg = ":server 352 " + client.getNickname() + " " 
							+ channelName + " "
							+ c->getUsername() + " " 
							+ maskedHost + " "
							+ "server "
							+ c->getNickname() + " " 
							+ prefix + status + " :0 "
							+ c->getRealname() + "\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
		}
	}

	std::string endMsg = ":server 315 " + client.getNickname() + " :End of WHO list\r\n";
	send(client.getFd(), endMsg.c_str(), endMsg.size(), 0);
}
