#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <string>
#include <poll.h>
#include "../client/Client.hpp"
#include "../channel/Channel.hpp"

class Server
{
	public:
		Server(int port, const std::string& password);
		~Server();

		void run();
	private:
	//core
		int _listenFd;
		int _port;
		std::string _password;
	
		std::vector<struct pollfd> _pfds;
		std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;
		std::map<std::string, int> _nickToFd;
	//setup
		void initListeningSocket();
	//Poll loop helpers
		void handleListeningSocket(short revents);
		void handleClientRead(int fd);
		void handleClientWrite(int fd);
		void disconnectClient(int fd, const std::string& reason);
	//Parsing & dispatch
		void processCompleteLines(Client& c);
		void dispatchCommand(Client& c, const std::string& line);
	//Commands
		void cmdPASS(Client& c, const std::vector<std::string>& args);
		void cmdNICK(Client& c, const std::vector<std::string>& args);
		void cmdUSER(Client& c, const std::vector<std::string>& args);
		void cmdPRIVMSG(Client& c, const std::vector<std::string>& args);
		void cmdNOTICE(Client& c, const std::vector<std::string>& args);
		void cmdQUIT(Client& c, const std::vector<std::string>& args);
	//Channel Commands
		void cmdJOIN(Client& c, const std::vector<std::string>& args);
		void cmdPART(Client& c, const std::vector<std::string>& args);
		void cmdTOPIC(Client& c, const std::vector<std::string>& args);
		void cmdMODE(Client& c, const std::vector<std::string>& args);
		void cmdINVITE(Client& c, const std::vector<std::string>& args);
		void cmdKICK(Client& c, const std::vector<std::string>& args);
	//Utilities
		void sendTo(Client& c, const std::string& msg);
		void sendToFd(int fd, const std::string& msg);
		void broadcastToChannel(const std::string& chan, const std::string& msg, int exceptFd);
	//Parsing helpers
		static void splitIrcLine(const std::string& line, std::string& cmd, std::vector<std::string>& params);
		static std::string toUpper(const std::string& s);

};

#endif
