#include "Client.hpp"

Client::Client() : fd(0), realname(""), username(""), nickname(""), _buff(""), authentificated(0), chanOn(NULL) {}

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
	chanOn = NULL;
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
Channel* Client::getChanOn() const { return chanOn; }
bool Client::getPass() const {return authentificated;}
std::string Client::getFullMask() const {return getNickn() + "!" + getUsern() + "@" + getRealn();}
void Client::appendToBuffer(const std::string& data) { _buff += data; }

std::vector<std::string> Client::extractCompleteCommands() 
{
	std::vector<std::string> commands;
	size_t pos;

	while ((pos = _buff.find('\n')) != std::string::npos) 
	{
		std::string cmd = _buff.substr(0, pos);
		_buff.erase(0, pos + 1); 				// +1 pour enlever le '\n'
		if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
			cmd.erase(cmd.size() - 1);
		commands.push_back(cmd);
	}
	return commands;
}

std::string Client::readMessage() 
{
	char buffer[1024];
	std::string msg;
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0) 
	{
		std::cerr << "Client disconnected: " << fd << std::endl;
		return "";
	}
	buffer[bytes] = '\0';
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

void Client::prompt(void)
{
	std::string p = getFullMask() + " ---------- IRC ---------- ";

	if (chanOn != NULL) 
	{
		p += "[" + chanOn->getName() + "]";

		if (!chanOn->getTopic().empty())
			p += "-[" + chanOn->getTopic() + "]";

		int opCount = chanOn->getOperatorCount();
		int memberCount = chanOn->getMemberCount();

		std::ostringstream oss;
		oss << " -> [Operators: " << opCount << " | Members: " << memberCount << "]";
		p += oss.str();
	} 
	else 
	{
		p += "[No channel]";
	}

	sendMessage(p);
	if (!getPass())
		sendMessage("Password :");
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

void Client::setChanOn(Channel* chan) 
{
	this->chanOn = chan;
}

void Client::setNickname(const std::string& nickname)
{
	this->nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
	this->username = username;
}
