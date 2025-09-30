//kullanıcı nickname belirler
// BİTTİ

#include "../server/Server.hpp"

bool invalidNick(std::string nick)
{
	if(nick.empty())
		return true;

	if(nick.size() > 9)
		return true;

	char first = nick[0];
	if(!isalpha(first) && first != '[' && first != ']' &&
		first != '\\' && first != '`' && first != '_' &&
		first != '^' && first != '{' && first != '|' &&
		first != '}')
		return true;

	for (size_t i = 1; i < nick.size(); i++)
	{
		char c = nick[i];
		if(!isalnum(c) && c != '-' && c != '[' && c != ']' &&
			c != '\\' && c != '`' && c != '_' &&
			c != '^' && c != '{' && c != '|' && c != '}')
			return true;
	}

	return false;
}

void Server::Nick(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = ":server 431 * NICK :No nickname given\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	
	if (!client.getPassGiven())
	{
		std::string msg = ":server 464 * :Password required\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	
	std::string nick = params[0];

	if(invalidNick(nick))
	{
		std::string msg = ":server 432 " + nick + " :Erroneous nickname\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(alreadyUseNick(nick))
	{
		std::string msg = ":server 433 " + nick + " :Nickname is already in use\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	std::string oldNick = client.getNickname();
	client.setNickname(nick);
	if (!oldNick.empty())
	{
		std::string nickChangeMsg = ":" + oldNick + "!" + client.getUsername() + "@" + client.getHostname() + " NICK :" + nick + "\r\n";

		std::vector<Channel*> chans = client.getChannels();
		if (chans.empty())
		{
			send(client.getFd(), nickChangeMsg.c_str(), nickChangeMsg.size(), 0);
			return;
		}
		for (size_t i = 0; i < chans.size(); ++i)
		{
			Channel* ch = chans[i];
			std::vector<Client*> members = ch->getMembers();
			for (size_t j = 0; j < members.size(); ++j)
			{
				Client* c = members[j];
				send(c->getFd(), nickChangeMsg.c_str(), nickChangeMsg.size(), 0);
			}
		}
	}
	else
	{
		std::string welcome = ":server 001 " + nick + " :Welcome to the IRC server\r\n";
		send(client.getFd(), welcome.c_str(), welcome.size(), 0);
	}
}