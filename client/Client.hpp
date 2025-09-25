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
		bool passGiven; //
		std::string nickname; //
		std::string username; //
		std::string hostname; //
		std::string servername; //
		std::string realname; //
		std::vector<Channel *> channels;
		
	public:
		Client(int _fd);
		~Client();

		void setFd(int _fd);
		void setLoggedIn(bool sign);
		void setPassGiven(bool sign);
		void setNickname(const std::string& _nickname);
		void setUsername(const std::string& _username);
		void setHostname(const std::string& _hostname);
		void setServername(const std::string& _servername);
		void setRealname(const std::string& _realname);

		int getFd() const;
		bool getLoggedIn() const;
		bool getPassGiven() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getHostname() const;
		const std::string& getServername() const;
		const std::string& getRealname() const;
        const std::vector<Channel *>& getChannels() const;

		void addChannel(Channel* channel);
        void removeChannel(Channel* channel);
		bool isInChannel(Channel* channel) const;
		std::string getPrefix() const;
};

#endif