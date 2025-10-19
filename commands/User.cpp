// kullanıcının username, ana makine adı, real name set eder
// BİTTİ

#include "../server/Server.hpp"

void Server::User(std::vector<std::string> params, Client &client)
{
	if (params.size() < 4) 
	{
		std::string msg = ":server 461 * USER :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	
	if (!client.getPassGiven())
	{
		std::string msg = ":server 464 * :Password required\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if(client.getNickname().empty())
	{
		std::string msg = ":server 451 * :NICK required\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	if (client.getLoggedIn())
	{
		std::string msg = ":server 462 " + client.getNickname() + " :You may not reregister\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}


	client.setUsername(params[0]);
	client.setHostname(params[1]);
	client.setServername(params[2]);
	client.setRealname(params[3]);
	
	std::string welcomeMsg = ":server 001 " + client.getNickname() + " :Welcome to the IRC Network\r\n";
	send(client.getFd(), welcomeMsg.c_str(), welcomeMsg.size(), 0);
	std::string hostMsg = ":server 002 " + client.getNickname() + " :Your host is server, running version 1.0\r\n";
	send(client.getFd(), hostMsg.c_str(), hostMsg.size(), 0);
	std::string createdMsg = ":server 003 " + client.getNickname() + " :This server was created today\r\n";
	send(client.getFd(), createdMsg.c_str(), createdMsg.size(), 0);
	
	client.setLoggedIn(true);
}