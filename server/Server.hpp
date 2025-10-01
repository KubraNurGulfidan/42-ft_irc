#ifndef SERVER_HPP
# define SERVER_HPP

# include "../channel/Channel.hpp"
# include "../client/Client.hpp"

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include <algorithm>
# include <unistd.h>
# include <cstdio>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <cstring>
# include <poll.h>
# include <fcntl.h>
# include <errno.h>


class Server
{
	private:
		int _port;
		int _serverFd;
		std::vector<pollfd> _pfds;
		std::string _password;
		std::map<std::string, Channel*> channels;
		std::vector<Client*> clients;
		bool isRun;

	public:
		Server(int port, const std::string& password);
		~Server();

		void commandHandler(std::string cmd, std::vector<std::string> params, Client &client);
		void Invite(std::vector<std::string> params, Client &client);
		void Join(std::vector<std::string> params, Client &client);
		void Kick(std::vector<std::string> params, Client &client);
		void List(std::vector<std::string> params, Client &client);
		void Mode(std::vector<std::string> params, Client &client);
		void Nick(std::vector<std::string> params, Client &client);
		void Notice(std::vector<std::string> params, Client &client);
		void Part(std::vector<std::string> params, Client &client);
		void Pass(std::vector<std::string> params, Client &client);
		void Privmsg(std::vector<std::string> params, Client &client);
		void Quit(std::vector<std::string> params, Client &client);
		void Topic(std::vector<std::string> params, Client &client);
		void User(std::vector<std::string> params, Client &client);
		void Who(std::vector<std::string> params, Client &client);
		void Ping(std::vector<std::string> params, Client &client);
		void Pong(std::vector<std::string> params, Client &client);

		std::string getPassword() const;
		bool alreadyUseNick(std::string nick);

		Channel* getChannelByName(const std::string& name);
		Client* getClientByNick(const std::string& nickname);
		Client* getClientByFd(int fd);
		void removeClient(Client* client);

		void handleClient(Client* client);
		int acceptClient();
		void start();
		void stop();
		bool setupServer();
		void runServer();
};

void parseIRCMessage(const std::string &input, std::vector<std::string>& params, std::string &command);

#endif