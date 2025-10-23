
#include "../server/Server.hpp"

void Server::User(std::vector<std::string> params, Client &client)
{
	if (params.size() < 4) 
	{
		std::string msg = ":server 461 * USER :Not enough parameters\r\n";
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
	
}