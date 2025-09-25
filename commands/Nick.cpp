//kullanıcı nickname belirler

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
		std::string msg = "461 NICK :No nickname given\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	
	if (!client.getPassGiven())
	{
		std::string msg = "464 :Password required\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	
	std::string nick = params[0];

	if(invalidNick(nick))
	{
		std::string msg = "432 " + nick + " :Erroneous nickname\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(alreadyUseNick(nick))
	{
		std::string msg = "433 " + nick + " :Nickname is already in use\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	client.setNickname(nick);
}