// servera bağlanırken şifre gönderir
// BİTTİ

#include "../server/Server.hpp"

void Server::Pass(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = ":server :server 461 * PASS :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (client.getLoggedIn())
	{
		std::string msg = ":server 462 * :You are already registered\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (params[0] != this->getPassword())
	{
		std::string msg = ":server 464 * :Password incorrect\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	client.setPassGiven(true);
}