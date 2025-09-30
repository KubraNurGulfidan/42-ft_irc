#include "Client.hpp"

Client::Client(int _fd): fd(_fd), loggedIn(false)  {}

Client::~Client()
{
	channels.clear();
}

void Client::setFd(int _fd) { this->fd = _fd; }
void Client::setLoggedIn(bool sign) { this->loggedIn = sign; }
void Client::setPassGiven(bool sign) { this->passGiven = sign; }
void Client::setNickname(const std::string& _nickname) { this->nickname = _nickname; }
void Client::setUsername(const std::string& _username) { this->username = _username; }
void Client::setRealname(const std::string& _realname) { this->realname = _realname; }
void Client::setServername(const std::string& _servername) { this->servername = _servername; }
void Client::setHostname(const std::string& _hostname) { this->hostname = _hostname; }

int Client::getFd() const { return this->fd; }
bool Client::getLoggedIn() const { return this->loggedIn; }
bool Client::getPassGiven() const { return this->passGiven; }
const std::string& Client::getNickname() const { return this->nickname; }
const std::string& Client::getUsername() const { return this->username; }
const std::string& Client::getRealname() const { return this->realname; }
const std::string& Client::getServername() const { return this->servername; }
const std::string& Client::getHostname() const { return this->hostname; }
const std::vector<Channel *>& Client::getChannels() const { return this->channels; }


void Client::addChannel(Channel* channel)
{
    if (channel && std::find(channels.begin(), channels.end(), channel) == channels.end())
        channels.push_back(channel);
}

void Client::removeChannel(Channel* channel)
{
    channels.erase(std::remove(channels.begin(), channels.end(), channel), channels.end());
}

bool Client::isInChannel(Channel* channel) const
{
    if (!channel)
		return false;

    std::vector<Channel*>::const_iterator it = std::find(channels.begin(), channels.end(), channel);
    return (it != channels.end());
}

std::string Client::getPrefix() const
{
    return getNickname() + "!" + getUsername() + "@" + getHostname();
}