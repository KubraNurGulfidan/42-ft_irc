#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <vector>
# include <algorithm>
# include <iostream>

class Channel;

class Client
{
	private:
		int fd;
		bool loggedIn;
		bool passGiven;
		bool away;
		bool isOperator;
		std::string nickname;
		std::string username;
		std::string hostname;
		std::string servername;
		std::string realname;
		std::string awayMessage;
		std::vector<Channel *> channels;
		
		time_t lastMessageTime;
		int messageCount;
		static const int MAX_MESSAGES_PER_SECOND = 5;
		
		std::vector<std::string> pendingMessages;
		
	public:
		std::string buffer;
		static const int MAX_BUFFER_SIZE = 8192;
		Client(int _fd);
		~Client();

		void setFd(int _fd);
		void setLoggedIn(bool sign);
		void setPassGiven(bool sign);
		void setAway(bool _away);
		void setIsOperator(bool _isOperator);
		void setNickname(const std::string& _nickname);
		void setUsername(const std::string& _username);
		void setHostname(const std::string& _hostname);
		void setServername(const std::string& _servername);
		void setRealname(const std::string& _realname);
		void setAwayMessage(const std::string& _awayMessage);

		int getFd() const;
		bool getLoggedIn() const;
		bool getPassGiven() const;
		bool getAway() const;
		bool getIsOperator() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getHostname() const;
		const std::string& getServername() const;
		const std::string& getRealname() const;
		const std::string& getAwayMessage() const;
        const std::vector<Channel *>& getChannels() const;

		void addChannel(Channel* channel);
        void removeChannel(Channel* channel);
		bool isInChannel(Channel* channel) const;
		std::string getPrefix() const;
		
		bool checkFloodProtection();
		void resetFloodCounter();
		bool isFlooding() const;
		
		void addPendingMessage(const std::string& message);
		void sendPendingMessages();
		void clearPendingMessages();
		bool hasPendingMessages() const;
};

#endif