#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <algorithm>
# include <iostream>

class Client;

class Channel
{
	private:
		std::vector<Client *> members;
		std::vector<Client *> admins;
		std::string channelName;
		std::string topic;
		std::string password;
		int userLimit;
		bool inviteOnly;

	public:
		Channel(const std::string& _channelName);
		~Channel();

		void setMemebers(std::vector<Client *>& clients);
		void setTopic(std::string newTopic);
		void setUserLimit(int limit);
		void setInviteOnly(bool value);
		
		std::vector<Client *>& getMembers();
		std::vector<Client *>& getAdmins();
		std::string getChannelName();
		std::string getTopic();
		int getUserLimit() const;
		
		bool isInviteOnly() const;

		void addClient(Client* client);
		void addAdmin(Client* admin);

		void removeClient(Client* client);
		void removeAdmin(Client* admin);

		bool hasMember(Client* client) const;
		bool hasAdmin(Client* client) const;
};

#endif