#include "server/Server.hpp"

int ft_port_check(const std::string& port)
{
	if (port.empty())
		return 1;
	for (size_t i = 0; i < port.length(); ++i)
    {
		if (!std::isdigit(static_cast<unsigned char>(port[i])))
			return 1;
	}
	unsigned long value = 0;
	for (size_t i = 0; i < port.length(); ++i)
    {
		value = value * 10 + static_cast<unsigned long>(port[i] - '0');
		if (value > 65535UL)
			return 1;
	}
	return (value >= 1UL && value <= 65535UL) ? 0 : 1;
}

int ft_password_check(const std::string& password)
{
	return password.empty() ? 1 : 0;
}

int main(int ac, char **av)
{
	if(ac != 3)
	{
		std::cerr << "The format must be ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	if(ft_port_check(av[1]) != 0)
	{
		std::cerr << "The port must be a number between 1 and 65535" << std::endl;
		return 1;
	}
	if(ft_password_check(av[2]) != 0)
	{
		std::cerr << "The password must be a string" << std::endl;
		return 1;
	}
	try
	{
		Server srv(atoi(av[1]), av[2]);
		// srv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Fatal: " << e.what() << std::endl;
		return 1;
	}
}

// #include <iostream>
// #include "client/Client.hpp"
// #include "channel/Channel.hpp"

// int main() 
// {
//     // 1) Client oluştur
//     Client alice(1);
//     Client bob(2);
// 	Client anna(3);
// 	alice.setNickname("alice");
// 	bob.setNickname("bob");
// 	anna.setNickname("anna");

// 	alice.setUsername("AliceUser");

//     std::cout << "Client 1: " << alice.getNickname() << " (fd=" << alice.getFd() << ")" << std::endl;
//     std::cout << "Client 2: " << bob.getNickname() << " (fd=" << bob.getFd() << ")" << std::endl;

//     // 2) Channel oluştur
//     Channel general("#general");
// 	general.setUserLimit(2);

//     std::cout << "Created channel: " << general.getChannelName() << std::endl;

//     // 3) Client'ları kanala ekle
//     general.addAdmin(&bob); // bob admin olacak
//     general.addClient(&alice);
// 	general.addClient(&anna);

//     // 4) Kanaldaki üyeleri listele
//     std::cout << "Members in " << general.getChannelName() << ": " << std::endl;
//     for (size_t i = 0; i < general.getMembers().size(); i++) {
//         std::cout << " - " << general.getMembers()[i]->getNickname() << std::endl;
//     }

//     // 5) Adminleri listele
//     std::cout << "Admins in " << general.getChannelName() << ": " << std::endl;
//     for (size_t i = 0; i < general.getAdmins().size(); i++) {
//         std::cout << " * " << general.getAdmins()[i]->getNickname() << std::endl;
//     }
// 	Channel deneme("#deneme");
// 	deneme.addClient(&alice);

//     // 6) Client hangi kanallarda?
//     std::cout << "Alice channels: ";
//     for (size_t i = 0; i < alice.getChannels().size(); i++) {
//         std::cout << alice.getChannels()[i]->getChannelName() << " ";
//     }
//     std::cout << std::endl;

//     std::cout << "Bob channels: ";
//     for (size_t i = 0; i < bob.getChannels().size(); i++) {
//         std::cout << bob.getChannels()[i]->getChannelName() << " ";
//     }
//     std::cout << std::endl;

// 	general.removeClient(&alice);
// 	std::cout << "Alice channels: ";
//     for (size_t i = 0; i < alice.getChannels().size(); i++) {
//         std::cout << alice.getChannels()[i]->getChannelName() << " ";
//     }
//     std::cout << std::endl;

// 	general.addClient(&anna);
// 	std::cout << "Members in " << general.getChannelName() << ": " << std::endl;
//     for (size_t i = 0; i < general.getMembers().size(); i++) {
//         std::cout << " - " << general.getMembers()[i]->getNickname() << std::endl;
//     }

// 	if (alice.isInChannel(&general)) {
// 		std::cout << "Alice kanalda!" << std::endl;
// 	}
// 	else
// 		std::cout << "Alice kanalda değil!" << std::endl;

	
// 	if (general.hasMember(&bob)) {
// 		std::cout << "Bob kanalda!" << std::endl;
// 	}
// 	else
// 		std::cout << "Bob kanalda değil!" << std::endl;
	
// 	if (general.hasAdmin(&anna)) {
// 		std::cout << "anna admin!" << std::endl;
// 	}
// 	else
// 		std::cout << "anna admin değil!" << std::endl;
	
// 	Server server;

// 	std::vector<std::string> params; // parametre yok
// 	std::string wrongCmd = "FOOBAR"; // bilinmeyen komut

// 	server.commandHandler(wrongCmd, params, alice);

// 	std::cout << "------------------------" << std::endl;

// 	std::string input = "MODE #kanal +k secret123";
//     std::string command;

//     parseIRCMessage(input, params, command);
// 	server.commandHandler(command, params, alice);

//     std::cout << "Command: " << command << std::endl;
//     for (size_t i = 0; i < params.size(); ++i) {
//         std::cout << "Param[" << i << "]: " << params[i] << std::endl;
//     }

//     return 0;
// }
