#include "Channel.hpp"

// Constructeur par défaut + paramétré + copie + affectation + Destructeur 
Channel::Channel() : name(""), topic(""), inviteOnly(false), password(""), maxUsers(50)
{
}
Channel::Channel(const std::string& channelName, const std::string& channelPassword, int maxUsers)
    : name(channelName), topic(""), inviteOnly(false), password(channelPassword), maxUsers(maxUsers)
{
}
Channel::Channel(const Channel& other)
    : name(other.name), topic(other.topic), members(other.members),
      inviteOnly(other.inviteOnly),
      password(other.password), maxUsers(other.maxUsers)
{
}
Channel& Channel::operator=(const Channel& other)
{
    if (this != &other) 
    {
        name = other.name;
        topic = other.topic;
        members = other.members;
        inviteOnly = other.inviteOnly;
        password = other.password;
        maxUsers = other.maxUsers;

    }
    return *this;
}
Channel::~Channel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-> Getters et setters --------------------------------------------------------------------------------------------------------------------------
std::string Channel::getName() const
{
    return name;
}
std::string Channel::getTopic() const
{
    return topic;
}
void Channel::setTopic(const std::string& newTopic)
{
    topic = newTopic;
}

//-> Membres -----------------------------------------------------------------------------------------------------------------------------------
bool Channel::addMember(const std::string& user, bool isOperator)
{
    if (members.size() >= static_cast<size_t>(maxUsers))
        return false;

    members[user] = isOperator;
    return true;
}

bool Channel::removeMember(const std::string& user)
{
    return members.erase(user) > 0;
}

bool Channel::isMember(const std::string& user) const
{
    return members.find(user) != members.end();
}

//-> Permissions -------------------------------------------------------------------------------------------------------------------------------
bool Channel::promoteToOperator(const std::string& user)
{
    if (members.find(user) != members.end()) {
        members[user] = true;
        return true;
    }
    return false;
}

bool Channel::demoteOperator(const std::string& user)
{
    if (members.find(user) != members.end()) {
        members[user] = false;
        return true;
    }
    return false;
}

//-> Modes -------------------------------------------------------------------------------------------------------------------------------------
void Channel::setInviteOnly(bool isInviteOnly)
{
    this->inviteOnly = isInviteOnly;
}

void Channel::setPassword(const std::string& newPassword)
{
    this->password = newPassword;
}

bool Channel::checkPassword(const std::string& inputPassword) const
{
    return inputPassword == password;
}

//-> Bannissement -------------------------------------------------------------------------------------------------------------------------------
int Channel::getMemberCount() const
{
    // Utilise size() dans <map> pour retourner le nombre de membre
    return members.size();
}

