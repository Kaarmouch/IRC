#include "Channel.hpp"

Channel::Channel() : name(""), topic(""), inviteOnly(false), topicRestricted(false), password(""), maxUsers(50) {}

Channel::Channel(const std::string& channelName, const std::string& channelPassword, int maxUsers)
    : name(channelName), topic(""), inviteOnly(false), topicRestricted(false), password(channelPassword), maxUsers(maxUsers) {}

Channel::Channel(const Channel& other)
    : name(other.name), topic(other.topic), members(other.members),
      inviteOnly(other.inviteOnly),topicRestricted(other.topicRestricted), password(other.password), 
      maxUsers(other.maxUsers) {}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other) 
    {
        name = other.name;
        topic = other.topic;
        members = other.members;
        inviteOnly = other.inviteOnly;
	topicRestricted = other.topicRestricted;
        password = other.password;
        maxUsers = other.maxUsers;
    }
    return *this;
}

Channel::~Channel() {}

//-> Getters et setters --------------------------------------------------------------------------------------------------------------------------
std::string Channel::getName() const
{
    return name;
}

std::string Channel::getTopic() const
{
    return topic;
}

bool Channel::getIOnly() const
{
	return inviteOnly;
}

bool Channel::getRTopic() const
{
        return topicRestricted;
}

int Channel::getMaxUsers() const
{
        return maxUsers;
}

std::string Channel::getPassword() const
{
        return password;
}

void Channel::setTopic(const std::string& newTopic)
{
    topic = newTopic;
}

void Channel::setIOnly(const bool mode)
{
	inviteOnly = mode;
}

void Channel::setRTopic(const bool mode)
{
	topicRestricted = mode;
}

void Channel::setMaxUsers(const unsigned int maxU)
{
	maxUsers = maxU;
}

void Channel::setPassword(const std::string &pass)
{
	password = pass;
}

void Channel::addInvite(Client* c)
{
	invitedNick.insert(c);
}

void Channel::removeInvite(Client* c) 
{
	invitedNick.erase(c);
}

bool Channel::isInvited(Client* c) 
{
    return invitedNick.find(c) != invitedNick.end();
}

int Channel::getOperatorCount() const 
{
    int count = 0;
    for (std::map<Client*, bool>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (it->second)
            count++;
    }
    return count;
}

//-> Membres -----------------------------------------------------------------------------------------------------------------------------------
bool Channel::addMember(Client* client, bool isOp) 
{
	if (members.find(client) != members.end())
		return false;

	if (inviteOnly && !isInvited(client))
		return false;

    if (maxUsers > 0 && (int)members.size() >= maxUsers)
    {
        client->sendMessage("Channel is full");
        return false;
    }

	members.insert(std::make_pair(client, isOp));
	return true;
}

bool Channel::removeMember(Client* client) 
{
    return members.erase(client) > 0;
}

bool Channel::isEmpty() const 
{
    return members.empty();
}

bool Channel::isMember(Client* client) const 
{
    return members.find(client) != members.end();
}

bool Channel::isOperator(Client* client) const 
{
    std::map<Client*, bool>::const_iterator it = members.find(client);
    if (it != members.end())
        return it->second;
    return false;
}

void Channel::sendList(Client *clin, const std::string& serverName)
{
	std::string list;

	for (std::map<Client*, bool>::const_iterator it = members.begin(); it != members.end(); )
	{
		if (it->second)
			list += "@";
		list += it->first->getNickn();
		if (++it != members.end())
			list += " ";
	}

	clin->sendMessage(":" + serverName + " 353 " + clin->getNickn() + " = " + this->name + " :" + list);
	clin->sendMessage(":" + serverName + " 366 " + clin->getNickn() + " " + this->name + " :End of /NAMES list.");
}

void	Channel::sendAll(Client *cli,const std::string& msg)
{
	std::string name = cli->getNickn();
	for (std::map<Client*, bool>::const_iterator it = members.begin(); it != members.end(); ++it) 
	{
		if (((it->first) != cli))
		{
			((it->first))->sendMessage(msg);
			((it->first))->prompt();
		}
	}
}

bool Channel::promoteToOperator(Client* client)
{
    std::map<Client*, bool>::iterator it = members.find(client);
    if (it != members.end()) 
    {
        it->second = true;
        return true;
    }
    return false;
}

bool Channel::demoteOperator(Client* client)
{
    std::map<Client*, bool>::iterator it = members.find(client);
    if (it != members.end()) 
    {
        it->second = false;
        return true;
    }
    return false;
}

//-> Utilitaires -------------------------------------------------------------------------------------------------------------------------------
bool Channel::checkPassword(const std::string& inputPassword) const
{
    return inputPassword == password;
}

int Channel::getMemberCount() const
{
    return members.size();
}

std::map<Client*, bool> Channel::getMembers()
{
    return this->members;
}