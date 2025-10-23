#include "Client.hpp"
#include <sys/socket.h>
#include <ctime>

Client::Client(int _fd): fd(_fd), loggedIn(false), passGiven(false), away(false), isOperator(false), lastMessageTime(time(NULL)), messageCount(0)  {}

Client::~Client()
{
	channels.clear();
}

void Client::setFd(int _fd) { this->fd = _fd; }
void Client::setLoggedIn(bool sign) { this->loggedIn = sign; }
void Client::setPassGiven(bool sign) { this->passGiven = sign; }
void Client::setAway(bool _away) { this->away = _away; }
void Client::setIsOperator(bool _isOperator) { this->isOperator = _isOperator; }
void Client::setNickname(const std::string& _nickname) { this->nickname = _nickname; }
void Client::setUsername(const std::string& _username) { this->username = _username; }
void Client::setRealname(const std::string& _realname) { this->realname = _realname; }
void Client::setServername(const std::string& _servername) { this->servername = _servername; }
void Client::setHostname(const std::string& _hostname) { this->hostname = _hostname; }
void Client::setAwayMessage(const std::string& _awayMessage) { this->awayMessage = _awayMessage; }

int Client::getFd() const { return this->fd; }
bool Client::getLoggedIn() const { return this->loggedIn; }
bool Client::getPassGiven() const { return this->passGiven; }
bool Client::getAway() const { return this->away; }
bool Client::getIsOperator() const { return this->isOperator; }
const std::string& Client::getNickname() const { return this->nickname; }
const std::string& Client::getUsername() const { return this->username; }
const std::string& Client::getRealname() const { return this->realname; }
const std::string& Client::getServername() const { return this->servername; }
const std::string& Client::getHostname() const { return this->hostname; }
const std::string& Client::getAwayMessage() const { return this->awayMessage; }
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

bool Client::checkFloodProtection()
{
    time_t currentTime = time(NULL);
    
    if (currentTime - lastMessageTime >= 1)
    {
        resetFloodCounter();
        lastMessageTime = currentTime;
    }
    
    messageCount++;
    
    if (messageCount > MAX_MESSAGES_PER_SECOND)
    {
        return false;
    }
    
    return true;
}

void Client::resetFloodCounter()
{
    messageCount = 0;
}

bool Client::isFlooding() const
{
    return messageCount > MAX_MESSAGES_PER_SECOND;
}

void Client::addPendingMessage(const std::string& message)
{
    pendingMessages.push_back(message);
}

void Client::sendPendingMessages()
{
    for (size_t i = 0; i < pendingMessages.size(); i++)
    {
        send(fd, pendingMessages[i].c_str(), pendingMessages[i].size(), 0);
    }
    clearPendingMessages();
}

void Client::clearPendingMessages()
{
    pendingMessages.clear();
}

bool Client::hasPendingMessages() const
{
    return !pendingMessages.empty();
}