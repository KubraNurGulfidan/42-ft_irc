#include "Channel.hpp"
#include "client/Client.hpp"

Channel::Channel() : inviteOnly(false), topicRestricted(false), userLimit(-1) {}
Channel::Channel(const std::string& n) : _name(n), inviteOnly(false), topicRestricted(false), userLimit(-1) {}

const std::string& Channel::getName() const
{
    return _name;
}

bool Channel::isMember(Client* c) const
{
    return _members.count(c) != 0;
}

bool Channel::isOperator(Client* c) const
{
    return _operators.count(c) != 0;
}

void Channel::addMember(Client* c, bool asOperator)
{
    _members.insert(c);
    if (asOperator)
        _operators.insert(c);
}

void Channel::removeMember(Client* c)
{
    _members.erase(c);
    _operators.erase(c);
    invited.erase(c);
}

void Channel::addOperator(Client* c)
{
    if (isMember(c))
        _operators.insert(c);
}

void Channel::removeOperator(Client* c)
{
    _operators.erase(c);
}

void Channel::membersSnapshot(std::set<Client*>& out) const
{
    out = _members;
}

std::size_t Channel::memberCount() const
{
    return _members.size();
}