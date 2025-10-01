#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

class Client;

class Channel
{
	private:
		std::string _name;
		std::set<Client*> _members;
		std::set<Client*> _operators;
	public:
		Channel();
		explicit Channel(const std::string& name);

		const std::string& getName() const;

		bool isMember(Client* c) const;
		bool isOperator(Client* c) const;//neden & değil de *
		
		void addMember(Client* c, bool asOperator);
		void removeMember(Client* c);

		void addOperator(Client* c);
		void removeOperator(Client* c);

		bool inviteOnly;
		bool topicRestricted;
		std::string key;
		int userLimit;

		std::set<Client*> invited;
		
		std::string topic;

		void membersSnapshot(std::set<Client*>& out) const;

		std::size_t memberCount() const;
};

#endif
