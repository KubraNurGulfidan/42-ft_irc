#include "Client.hpp"

Client::Client() : _fd(-1) , hasPassed(false), hasNick(false), hasUser(false), isRegistered(false) {}
Client::Client(int fd) : _fd(fd), hasPassed(false), hasNick(false), hasUser(false), isRegistered(false) {}

int Client::getFd() const
{
    return _fd;
}

void Client::enqueue(const std::string& msg) 
{
    outputQueue.push_back(msg);
}

bool Client::hasPendingWrite() const
{
    return !outputQueue.empty();
}

std::string& Client::frontOut()
{
    return outputQueue.front();
}

void Client::popFrontOut()
{
    outputQueue.pop_front();
}