#include "server/Server.hpp"

// int main(int ac, char **av)
// {
// 	if(ac != 3)
// 	{
// 		std::cerr << "The format must be ./ircserv <port> <password>" << std::endl;
// 		return 1;
// 	}
// 	std::cout << "Port     : " << av[1] << std::endl;
// 	std::cout << "Password : " << av[2] << std::endl;
// }

#include <iostream>
#include "client/Client.hpp"
#include "channel/Channel.hpp"

int main() {
    // 1) Client oluştur
    Client alice(1);
    Client bob(2);
	Client anna(3);
	alice.setNickname("alice");
	bob.setNickname("bob");
	anna.setNickname("anna");

	alice.setUsername("AliceUser");

    std::cout << "Client 1: " << alice.getNickname() << " (fd=" << alice.getFd() << ")" << std::endl;
    std::cout << "Client 2: " << bob.getNickname() << " (fd=" << bob.getFd() << ")" << std::endl;

    // 2) Channel oluştur
    Channel general("#general");
	general.setUserLimit(2);

    std::cout << "Created channel: " << general.getChannelName() << std::endl;

    // 3) Client'ları kanala ekle
    general.addAdmin(&bob); // bob admin olacak
    general.addClient(&alice);
	general.addClient(&anna);

    // 4) Kanaldaki üyeleri listele
    std::cout << "Members in " << general.getChannelName() << ": " << std::endl;
    for (size_t i = 0; i < general.getMembers().size(); i++) {
        std::cout << " - " << general.getMembers()[i]->getNickname() << std::endl;
    }

    // 5) Adminleri listele
    std::cout << "Admins in " << general.getChannelName() << ": " << std::endl;
    for (size_t i = 0; i < general.getAdmins().size(); i++) {
        std::cout << " * " << general.getAdmins()[i]->getNickname() << std::endl;
    }
	Channel deneme("#deneme");
	deneme.addClient(&alice);

    // 6) Client hangi kanallarda?
    std::cout << "Alice channels: ";
    for (size_t i = 0; i < alice.getChannels().size(); i++) {
        std::cout << alice.getChannels()[i]->getChannelName() << " ";
    }
    std::cout << std::endl;

    std::cout << "Bob channels: ";
    for (size_t i = 0; i < bob.getChannels().size(); i++) {
        std::cout << bob.getChannels()[i]->getChannelName() << " ";
    }
    std::cout << std::endl;

	general.removeClient(&alice);
	std::cout << "Alice channels: ";
    for (size_t i = 0; i < alice.getChannels().size(); i++) {
        std::cout << alice.getChannels()[i]->getChannelName() << " ";
    }
    std::cout << std::endl;

	general.addClient(&anna);
	std::cout << "Members in " << general.getChannelName() << ": " << std::endl;
    for (size_t i = 0; i < general.getMembers().size(); i++) {
        std::cout << " - " << general.getMembers()[i]->getNickname() << std::endl;
    }

	if (alice.isInChannel(&general)) {
		std::cout << "Alice kanalda!" << std::endl;
	}
	else
		std::cout << "Alice kanalda değil!" << std::endl;

	
	if (general.hasMember(&bob)) {
		std::cout << "Bob kanalda!" << std::endl;
	}
	else
		std::cout << "Bob kanalda değil!" << std::endl;
	
	if (general.hasAdmin(&anna)) {
		std::cout << "anna admin!" << std::endl;
	}
	else
		std::cout << "anna admin değil!" << std::endl;
	
	Server server;

	std::vector<std::string> params; // parametre yok
	std::string wrongCmd = "FOOBAR"; // bilinmeyen komut

	server.commandHandler(wrongCmd, params, alice);

	std::cout << "------------------------" << std::endl;

	std::string input = ":Alice!user@host PRIVMSG #general :Hello everyone";
    std::string command;

    parseIRCMessage(input, command, params);

    std::cout << "Command: " << command << std::endl;
    for (size_t i = 0; i < params.size(); ++i) {
        std::cout << "Param[" << i << "]: " << params[i] << std::endl;
    }

    return 0;
}
