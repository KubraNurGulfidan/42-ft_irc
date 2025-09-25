#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password), isRun(false) {}

Server::~Server() {}

std::string Server::getPassword() const { return _password; }

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

void Server::removeClient(Client *client)
{
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); )
	{
		Channel* ch = it->second;
		ch->removeClient(client);

		if (ch->getMembers().empty())
		{
			delete ch;
			channels.erase(it++);
		}
		else
			++it;
	}

	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == client)
		{
			clients.erase(it);
			break;
		}
	}
	delete client;
}
