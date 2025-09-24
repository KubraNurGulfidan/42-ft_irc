#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

std::string Server::getPassword() { return password; }

bool Server::alreadyUseNick(std::string nick)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if(clients[i]->getNickname() == nick)
			return true;
	}
	return false;	
}

Channel* Server::getChannelByName(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = channels.find(name);
	if (it != channels.end())
		return it->second;
	return NULL;
}

Client* Server::getClientByNick(const std::string& nickname)
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it)->getNickname() == nickname)
            return *it;
    }
    return NULL;
}

void Server::removeClient(Client* client)
{
    if (!client) return;

    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); )
	{
        Channel* ch = it->second;
        if (ch->hasMember(client))
		{
            ch->removeClient(client);
            if (ch->getMembers().empty())
			{
                delete ch;
                it = channels.erase(it);
                continue;
            }
        }
        ++it;
    }

    for (std::vector<Client*>::iterator itc = clients.begin(); itc != clients.end(); ++itc) {
        if (*itc == client) {
            clients.erase(itc);
            break;
        }
    }

    int fd = client->getFd();
    if (fd >= 0) {
        close(fd);
    }
    delete client;
}