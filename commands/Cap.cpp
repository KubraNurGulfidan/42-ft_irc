
#include "../server/Server.hpp"

void Server::Cap(std::vector<std::string> params, Client &client)
{
	(void)params;
	std::string endMsg = ":server 315 " + client.getNickname() + " :End of WHO list\r\n";
	send(client.getFd(), endMsg.c_str(), endMsg.size(), 0);
}