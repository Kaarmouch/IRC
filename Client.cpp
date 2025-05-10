#include "Client.hpp"

Client::Client() : fd(0), realname(""), username(""), nickname(""), authentificated(0), chanOn("No Channel") {}

Client::Client(const Client &copy) : fd(copy.fd), realname(copy.realname), username(copy.username), nickname(copy.nickname), authentificated(copy.authentificated), chanOn(copy.chanOn) {}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		std::cout << nickname << " a ete copié" << std::endl;
		fd = other.fd;
		realname = other.realname;
		username = other.username;
		nickname = other.nickname + "2";
		chanOn = other.chanOn;
		authentificated = other.authentificated;
	}
	return *this;
}

Client::~Client()
{
	std::cout << nickname << " a ete supp" << std::endl;
}

Client::Client(int file_d)
{
	fd = file_d;

	std::ostringstream oss;
	oss << "client_" << fd;
	nickname = oss.str();
	realname = "client";
	username = "client";
	authentificated = 0;
	chanOn = "No channel";
}

int Client::getFd() const
{
        if (fd)
                return fd;
        else
                return -1;
}


std::string Client::getRealn() const {return realname;}
std::string Client::getUsern() const {return username;}
std::string Client::getNickn() const {return nickname;}
std::string Client::getChanOn() const {return chanOn;}
bool Client::getPass() const {return authentificated;}


std::string Client::readMessage() 
{
	char buffer[1024];
	std::string msg;
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0) {
		std::cerr << "Client disconnected: " << fd << std::endl;
		return "";
	}

	buffer[bytes] = '\0';
	//buff += buffer;
	msg = buffer;
	
	return msg;
}

void Client::sendMessage(const std::string& message) 
{
	std::string formatted = message;
	if (formatted.find("\r\n") == std::string::npos)
		formatted += "\r\n";

	send(fd, formatted.c_str(), formatted.size(), 0);
}


void Client::setIpAdd(const std::string& ipAddr)
{
	realname = ipAddr;
	std::cout << "address set to " << realname << std::endl;
}

void Client::setPass(void)
{
	authentificated = 1;
}

void Client::setChanOn(const std::string& chanOn)
{
        this->chanOn = chanOn;
}


void Client::setNickname(const std::string& nickname)
{
	this->nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
	this->username = username;
}
/*
void Client::joinChannel(std::string channelName)
{
	this->channels.push_back(channelName);
}

ivoid Client::leaveChannel(std::string channelName)
{
	for (std::vecotr<std::string>::iterator i_name = channels.begin(); i_name != channels.end(); i_name++)
	{
		if (i_name = channelName)
		{
			channel.erase(i_name);
			break;
		}
	}
}*/
