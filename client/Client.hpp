#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <deque>
#include <set>

class Client
{
	public:
		Client();
		explicit Client(int fd);

		int getFd() const;

		bool hasPassed;
		bool hasNick;
		bool hasUser;
		bool isRegistered;

		std::string nickname;
		std::string username;
		std::string realname;
		std::string hostname;

		std::string inputBuffer;
		std::deque<std::string> outputQueue;

		std::set<std::string> joinedChannels;

		void enqueue(const std::string& msg);
		bool hasPendingWrite() const;
		std::string& frontOut();
		void popFrontOut();
	private:
		int _fd;
};

#endif
