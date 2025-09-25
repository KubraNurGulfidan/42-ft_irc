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
            // Trailing parametre: tüm geri kalan
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
}

void Server::commandHandler(std::string cmd, std::vector<std::string> params, Client &client)
{
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
	else if (cmd == "NICK")
		Nick(params, client);
	else if (cmd == "NOTICE")
		Notice(params, client);
	else if (cmd == "PART")
		Part(params, client);
	else if (cmd == "PASS")
		Pass(params, client);
	else if(cmd == "PRIVMSG")
		Privmsg(params, client);
	else if (cmd == "QUIT")
		Quit(params, client);
	else if (cmd == "TOPIC")
		Topic(params, client);
	else if (cmd == "USER")
		User(params, client);
	else if(cmd == "WHO")
		Who(params, client);
	else
	{
		std::string msg = "421 " + client.getNickname() + " " + cmd + " :Unknown command\r\n";
		// send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << msg << std::endl;
	}
}