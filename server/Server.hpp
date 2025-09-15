#ifndef SERVER_HPP
# define SERVER_HPP

#include "../channel/Channel.hpp"
#include "../client/Client.hpp"

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include <algorithm>


class Server
{
	private:
		int port;
		int serverFd;
		std::string password;
		std::map<std::string, Channel*> channels;
		std::vector<Client*> clients;
		bool isRun;

	public:
		Server();
		~Server();

		void commandHandler(std::string cmd, std::vector<std::string> params, Client &client);
		// void Invite(std::vector<std::string> params, Client &client);
		// void Join(std::vector<std::string> params, Client &client);
		// void Kick(std::vector<std::string> params, Client &client);
		// void List(std::vector<std::string> params, Client &client);
		// void Mode(std::vector<std::string> params, Client &client);
		// void Nick(std::vector<std::string> params, Client &client);
		// void Notice(std::vector<std::string> params, Client &client);
		// void Part(std::vector<std::string> params, Client &client);
		// void Pass(std::vector<std::string> params, Client &client);
		// void Privmsg(std::vector<std::string> params, Client &client);
		// void Quit(std::vector<std::string> params, Client &client);
		// void Topic(std::vector<std::string> params, Client &client);
		// void User(std::vector<std::string> params, Client &client);
		// void Who(std::vector<std::string> params, Client &client);

};

void parseIRCMessage(const std::string &input, std::string &command, std::vector<std::string> &params);

#endif