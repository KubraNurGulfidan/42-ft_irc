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
		std::vector<Client *> invitedClients;
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
		void setPassword(std::string newPassword);
		
		std::vector<Client *>& getMembers();
		std::vector<Client *>& getAdmins();
		std::string getChannelName();
		std::string getTopic();
		std::string getPassword();
		int getUserLimit() const;
		
		bool isInviteOnly() const;

		void addClient(Client* client);
		void addAdmin(Client* admin);
		void addInvited(Client* client);

		void removeClient(Client* client);
		void removeAdmin(Client* admin);
		void removeTopic();
		void removePassword();
		void removeUserLimit();

		bool hasMember(Client* client) const;
		bool hasAdmin(Client* client) const;
		bool hasInvited(Client* client) const;
};

#endif