#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password), isRun(false) {}

Server::~Server()
{
    if (_serverFd != -1)
        close(_serverFd);

    for (size_t i = 0; i < clients.size(); i++)
    {
        delete clients[i];
    }
    clients.clear();

    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        delete it->second;
    }
    channels.clear();

    _pfds.clear();
}

void Server::handleClient(Client* client)
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(client->getFd(), buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            std::cout << "Client disconnected (fd: " << client->getFd() << ")" << std::endl;
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        else
            perror("recv failed");

        removeClient(client);
        return;
    }

    std::string message(buffer);
    std::cout << "Received message from fd " << client->getFd() << ": " << message << std::endl;

    std::vector<std::string> params;
    std::string command;
    parseIRCMessage(message, params, command);
    commandHandler(command, params, *client);
}

void Server::start()
{
    if (!setupServer())
        return;
    runServer();
}

void Server::stop()
{
    isRun = false;
}

bool Server::setupServer()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return false;
    }
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt SO_REUSEADDR failed");
        close(server_socket);
        return false;
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt SO_KEEPALIVE failed");
        close(server_socket);
        return false;
    }
	
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        return false;
    }
    if (listen(server_socket, 5) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        return false;
    }

    int flags = fcntl(server_socket, F_GETFL, 0);
    if (flags < 0 || fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("fcntl failed for server socket");
        close(server_socket);
        return false;
    }
    _serverFd = server_socket;
    isRun = true;
    std::cout << "Server started on port " << _port << std::endl;
    return true;
}

void Server::runServer()
{
	_pfds.clear();
    pollfd serverPollFd;
	memset(&serverPollFd, 0, sizeof(serverPollFd));
    serverPollFd.fd = _serverFd;
    serverPollFd.events = POLLIN;
    _pfds.push_back(serverPollFd);

    while (isRun)
    {
        int activity = poll(_pfds.data(), _pfds.size(), 1000); 
        if (activity < 0)
        {
            perror("poll failed");
            break;
        }

        if (_pfds[0].revents & POLLIN)
        {
            int clientFd = acceptClient();
            if (clientFd > 0)
            {
                pollfd clientPollFd;
				memset(&clientPollFd, 0, sizeof(clientPollFd));
                clientPollFd.fd = clientFd;
                clientPollFd.events = POLLIN;
                _pfds.push_back(clientPollFd);
            }
        }

        for (size_t i = 1; i < _pfds.size(); )
        {
            if (_pfds[i].revents & POLLIN)
            {
                Client* client = getClientByFd(_pfds[i].fd);
                if (client)
                {
                    handleClient(client);
                    if (!getClientByFd(_pfds[i].fd))
                    {
                        _pfds.erase(_pfds.begin() + i);
                        continue;
                    }
                }
                else
                {
                    _pfds.erase(_pfds.begin() + i);
                    continue; 
                }
            }
            else if (_pfds[i].revents & (POLLHUP | POLLERR))
            {
                Client* client = getClientByFd(_pfds[i].fd);
                if (client)
                {
                    std::cout << "Client connection lost (fd: " << _pfds[i].fd << ")" << std::endl;
                    removeClient(client);
                }
                _pfds.erase(_pfds.begin() + i);
                continue;
            }
            i++;
        }
    }
    close(_serverFd);
    _serverFd = -1;
    isRun = false;
}
int Server::acceptClient()
{
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return -1;
        perror("Accept failed");
        return -1;
    }
    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags < 0 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("fcntl failed");
        close(clientFd);
        return -1;
    }

    std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << " (fd: " << clientFd << ")" << std::endl;

    Client* newClient = new Client(clientFd);
    clients.push_back(newClient);

    return clientFd;
}

Client* Server::getClientByFd(int fd)
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it)->getFd() == fd)
            return *it;
    }
    return NULL;
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
	int fd = client->getFd();

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
	close(fd);
	delete client;
}
