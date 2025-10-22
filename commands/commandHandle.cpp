#include "../server/Server.hpp"

void parseIRCMessage(const std::string &input, std::vector<std::string>& params, std::string &command)
{
	params.clear();
	std::string line = input;
	
	if (!line.empty() && line[0] == ':')
	{
		size_t spacePos = line.find(' ');
		if (spacePos != std::string::npos)
			line = line.substr(spacePos + 1);
	}

	std::istringstream iss(line);
    iss >> command;

	std::string param;
    while (iss >> param)
	{
		if (param[0] == ':')
		{
			std::string trailing = param.substr(1);
			std::string rest;
			getline(iss, rest);
			trailing += rest;
			params.push_back(trailing);
			break;
		}
		else
			params.push_back(param);
	}
	
	if (command == "JOIN" || command == "PART" || command == "KICK" || command == "MODE")
	{
		if (!params.empty())
		{
			std::string firstParam = params[0];
			std::vector<std::string> splitParams;
			
			size_t start = 0;
			size_t commaPos = firstParam.find(',');
			
			while (commaPos != std::string::npos)
			{
				splitParams.push_back(firstParam.substr(start, commaPos - start));
				start = commaPos + 1;
				commaPos = firstParam.find(',', start);
			}
			splitParams.push_back(firstParam.substr(start));
			
			params.erase(params.begin());
			params.insert(params.begin(), splitParams.begin(), splitParams.end());
		}
	}
	
	if (command == "USER" && params.size() >= 4)
	{
		std::string originalLine = input;
		if (!originalLine.empty() && originalLine[0] == ':')
		{
			size_t spacePos = originalLine.find(' ');
			if (spacePos != std::string::npos)
				originalLine = originalLine.substr(spacePos + 1);
		}
		
		std::istringstream originalIss(originalLine);
		std::string cmd;
		originalIss >> cmd;
		std::string p1, p2, p3, p4;
		originalIss >> p1 >> p2 >> p3 >> p4;
		
		if (p4.empty() || p4[0] != ':')
		{
			params.clear();
			params.push_back("INVALID");
		}
	}
}

void Server::checkRegistration(Client &client)
{
    if (!client.getNickname().empty() && !client.getUsername().empty() && client.getPassGiven() && !client.getLoggedIn())
    {
        client.setLoggedIn(true);
        
        std::string fullmask = client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
        
		std::string welcomeMsg = ":server 001 " + client.getNickname() + " :Welcome to the IRC Network\r\n" + fullmask + "\r\n";
		send(client.getFd(), welcomeMsg.c_str(), welcomeMsg.size(), 0);
		std::string hostMsg = ":server 002 " + client.getNickname() + " :Your host is server, running version 1.0\r\n";
		send(client.getFd(), hostMsg.c_str(), hostMsg.size(), 0);
		std::string createdMsg = ":server 003 " + client.getNickname() + " :This server was created today\r\n";
		send(client.getFd(), createdMsg.c_str(), createdMsg.size(), 0);
	}
}

void Server::commandHandler(std::string cmd, std::vector<std::string> params, Client &client)
{
	if(cmd == "CAP")
	{
		Cap(params, client);
		return;
	}
	if (cmd == "PASS")
	{
		Pass(params, client);
		checkRegistration(client);
		return;
	}
	if (cmd == "NICK")
	{
		Nick(params, client);
		checkRegistration(client);
		return;
	}
	else if (cmd == "USER")
	{
		User(params, client);
		checkRegistration(client);
		return;
	}

	if(cmd == "PING")
	{
		Ping(params, client);
		return;
	}
	else if(cmd == "PONG")
	{
		Pong(params, client);
		return;
	}
	else if (cmd == "QUIT")
	{
		Quit(params, client);
		return;
	}
		
	if (cmd == "INVITE" || cmd == "JOIN" || cmd == "KICK" || cmd == "LIST" || 
		cmd == "MODE" ||  cmd == "NICK" || cmd == "NOTICE" || cmd == "PART" || 
		cmd == "PRIVMSG" || cmd == "TOPIC" || cmd == "WHO" || cmd == "USER")
	{
		if (!client.getLoggedIn())
		{
			std::string msg = ":server 451 " + client.getNickname() + " :You have not registered\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
	}
	else
	{
		std::string msg = ":server 421 * " + client.getNickname() + " " + cmd + " :Unknown command\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
	}

	if (cmd == "INVITE")
		Invite(params, client);
	else if (cmd == "JOIN")
		Join(params, client);
	else if (cmd == "KICK")
		Kick(params, client);
	else if (cmd == "LIST")
		List(params, client);
	else if(cmd == "MODE")
		Mode(params, client);
	else if (cmd == "NOTICE")
		Notice(params, client);
	else if (cmd == "PART")
		Part(params, client);
	else if(cmd == "PRIVMSG")
		Privmsg(params, client);
	else if (cmd == "TOPIC")
		Topic(params, client);
	else if(cmd == "WHO")
		Who(params, client);
}