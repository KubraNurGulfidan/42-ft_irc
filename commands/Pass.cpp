// servera bağlanırken şifre gönderir

#include "../server/Server.hpp"

void Server::Pass(std::vector<std::string> params, Client &client)
{
	if (params.empty()) 
	{
		std::string msg = "461 PASS :Not enough parameters\r\n";
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

	if (params[0] != this->getPassword())
    {
        std::string msg = "464 :Password incorrect\r\n";
        // send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << msg << std::endl;
		return;
    }

	client.setPassGiven(true);
}