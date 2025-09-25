# include "Channel.hpp"
# include "../client/Client.hpp"

Channel::Channel(const std::string& _channelName)
	: channelName(_channelName), topic(""), password(""), userLimit(100), inviteOnly(false) {}

Channel::~Channel() {}

void Channel::setMemebers(std::vector<Client *>& clients) { members = clients; }
void Channel::setTopic(std::string newTopic) { topic = newTopic; }
void Channel::setPassword(std::string newPassword) { password = newPassword; }
void Channel::setUserLimit(int limit) { userLimit = limit; }
void Channel::setInviteOnly(bool value) { inviteOnly = value; }

std::vector<Client *>& Channel::getMembers() { return members; }
std::vector<Client *>& Channel::getAdmins() { return admins; }
std::string Channel::getChannelName() { return channelName; }
std::string Channel::getTopic() { return topic; }
std::string Channel::getPassword() { return password; }
int Channel::getUserLimit() const { return userLimit; }

bool Channel::isInviteOnly() const { return inviteOnly; }

void Channel::addClient(Client* client)
{
	if (members.size() == (long unsigned int)userLimit)
	{
		std::cout << client->getNickname() << " couldn't add channel. Channel limit is full!" << std::endl;
		return;
	}
	if (client && std::find(members.begin(), members.end(), client) == members.end()) 
	{
		members.push_back(client);
		client->addChannel(this);
	}
}

void Channel::addAdmin(Client* admin)
{
	if (admin && std::find(admins.begin(), admins.end(), admin) == admins.end())
	{
		admins.push_back(admin);
		addClient(admin);
	}
}

void Channel::addInvited(Client* client)
{
	if (client && std::find(invitedClients.begin(), invitedClients.end(), client) == invitedClients.end())
	{
		invitedClients.push_back(client);
		addClient(client);
	}
}

void Channel::removeClient(Client* client)
{
	if (!client)
		return;

	std::vector<Client*>::iterator it = std::find(members.begin(), members.end(), client);
	if (it != members.end())
	{
		members.erase(it);
		client->removeChannel(this);
	}
	removeAdmin(client);
}

void Channel::removeAdmin(Client* admin)
{
	if (!admin)
		return;

	std::vector<Client*>::iterator it = std::find(admins.begin(), admins.end(), admin);
	if (it != admins.end())
		admins.erase(it);
}

void Channel::removeTopic() { topic.clear(); }
void Channel::removePassword() { password.clear(); }
void Channel::removeUserLimit() { userLimit = 1000; }

bool Channel::hasMember(Client* client) const
{
	if (!client)
		return false;

	std::vector<Client*>::const_iterator it = std::find(members.begin(), members.end(), client);
	return (it != members.end());
}

bool Channel::hasAdmin(Client* client) const
{
	if (!client)
		return false;

	std::vector<Client*>::const_iterator it = std::find(admins.begin(), admins.end(), client);
	return (it != admins.end());
}

bool Channel::hasInvited(Client* client) const
{
	if (!client)
		return false;

	std::vector<Client*>::const_iterator it = std::find(invitedClients.begin(), invitedClients.end(), client);
	return (it != invitedClients.end());
}

void Channel::broadcast(const std::string &msg, int senderFd)
{
    for (size_t i = 0; i < members.size(); i++)
	{
        if (members[i]->getFd() != senderFd) 
            send(members[i]->getFd(), msg.c_str(), msg.size(), 0);
    }
}

