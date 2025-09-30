//kanala kullanıcı davet etmek için
// BİTTİ

#include "../server/Server.hpp"

void Server::Invite(std::vector<std::string> params, Client &client)
{
	if (params.size() < 2)
	{
		std::string msg = ":server 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string channelName = params[1];
	Channel *newChannel = getChannelByName(channelName);
	if (!newChannel)
	{
		std::string msg = ":server 403 * " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(!newChannel->hasMember(&client))
	{
		std::string msg = ":server 442 " + client.getNickname() + " " + channelName + " :Not on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (newChannel->isInviteOnly() && !newChannel->hasAdmin(&client))
	{
		std::string msg = ":server 482 " + client.getNickname() + " " + params[1] + " :You're not channel operator\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	Client* target = getClientByNick(params[0]);
	if (!target)
	{
		std::string msg = ":server 401 " + client.getNickname() + " " + params[0] + " :No such nick/channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (newChannel->hasMember(target))
	{
		std::string msg = ":server 443 " + client.getNickname() + " " + params[0] + " " + params[1] + " :is already on channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string inviteMsg = ":" + client.getNickname() + "!" + client.getUsername() +
						"@" + client.getHostname() + " INVITE " + target->getNickname() +
						" :" + channelName + "\r\n";
	send(target->getFd(), inviteMsg.c_str(), inviteMsg.size(), 0);

	std::string confirmMsg = ":server 341 " + client.getNickname() +
                         " " + target->getNickname() + " " + channelName + "\r\n";
	send(client.getFd(), confirmMsg.c_str(), confirmMsg.size(), 0);
}