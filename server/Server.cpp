#include "Server.hpp"

#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password), isRun(false) {}

Server::~Server()
{
    if (_serverFd != -1)
        close(_serverFd);
    for (size_t i = 0; i < clients.size(); i++)
        delete clients[i];
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
        delete it->second;
}

void Server::handleClient(Client* client)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(client->getFd(), buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0)
    {
        removeClient(client);
        return;
    }

    std::string message(buffer);
    std::cout << "Received message: " << message << std::endl;

    std::vector<std::string> params;
    std::string command;
    parseIRCMessage(message, params, command);
    commandHandler(command, params, *client);
}

void Server::start()
{
    // Soket oluşturma
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Adres yapısını ayarlama
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    // Soketi bağlama
    if (bind(_serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(_serverFd);
        exit(EXIT_FAILURE);
    }

    // Dinleme
    if (listen(_serverFd, 10) < 0)
    {
        perror("Listen failed");
        close(_serverFd);
        exit(EXIT_FAILURE);
    }

    isRun = true;
    std::cout << "Server started on port " << _port << std::endl;

    // Olay döngüsü için hazırlık
    fd_set readFds;
    int maxFd = _serverFd;

    while (isRun)
    {
        FD_ZERO(&readFds);
        FD_SET(_serverFd, &readFds);

        // Tüm istemci soketlerini set'e ekle
        for (size_t i = 0; i < clients.size(); ++i)
        {
            int clientFd = clients[i]->getFd();
            FD_SET(clientFd, &readFds);
            if (clientFd > maxFd)
                maxFd = clientFd;
        }

        // select ile aktif soketleri kontrol et
        int activity = select(maxFd + 1, &readFds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("Select failed");
            continue;
        }

        // Yeni bağlantıları kabul et
        if (FD_ISSET(_serverFd, &readFds))
            acceptClient();

        // Mevcut istemcilerden gelen verileri işle
        for (size_t i = 0; i < clients.size(); ++i)
        {
            int clientFd = clients[i]->getFd();
            if (FD_ISSET(clientFd, &readFds))
                handleClient(clients[i]);
        }
    }
}

void Server::acceptClient()
{
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd < 0)
    {
        perror("Accept failed");
        return;
    }

    std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
    Client* newClient = new Client(clientFd);
    clients.push_back(newClient);
}

std::string Server::getPassword() const { return _password; }

bool Server::alreadyUseNick(std::string nick)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if(clients[i]->getNickname() == nick)
			return true;
	}
	return false;	
}

Channel* Server::getChannelByName(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = channels.find(name);
	if (it != channels.end())
		return it->second;
	return NULL;
}

Client* Server::getClientByNick(const std::string& nickname)
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it)->getNickname() == nickname)
            return *it;
    }
    return NULL;
}

void Server::removeClient(Client *client)
{
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); )
	{
		Channel* ch = it->second;
		ch->removeClient(client);

		if (ch->getMembers().empty())
		{
			delete ch;
			channels.erase(it++);
		}
		else
			++it;
	}

	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == client)
		{
			clients.erase(it);
			break;
		}
	}
	delete client;
}
