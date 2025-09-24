// kullanıcının username, ana makine adı, real name set eder

#include "../server/Server.hpp"

void Server::User(std::vector<std::string> params, Client &client)
{
	if (params.size() < 4) 
	{
		std::string msg = "461 " + client.getNickname() + "USER :Not enough parameters\r\n";
		// send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << msg << std::endl;
		return;
	}
	
	if (!client.getPassGiven())
	{
		std::string msg = "464 :Password required\r\n";
		// send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << msg << std::endl;
		return;
	}

	if(client.getNickname().empty())
	{
		std::string msg = "451 :NICK required\r\n";
		// send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << msg << std::endl;
		return;
	}

	if (client.getLoggedIn())
	{
		std::string msg = "462 " + client.getNickname() + " :You may not reregister\r\n";
		// send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << msg << std::endl;
		return;
	}

	client.setUsername(params[0]);
	client.setHostname(params[1]);
	client.setServername(params[2]);
	client.setRealname(params[3]);
	client.setLoggedIn(true);
}