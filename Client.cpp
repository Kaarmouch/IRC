#include <Client.hpp>

Client::Client() : fd(0), name(""), addr(""), nickname("") {}

Client::Client(const Client &copy) : fd(fd.copy), name(name.copy), ip_adrs("") {}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		fd = other.fd;
		name = other.name;
		ip_adrs = other.ip_adrs;
	}
	return *this;
}

Server::~Server()
{
	std::cout << "Client "<< name << "disconected"<< std::endl;
}

Client::Client(int file_d)
{
	fd = file_d;
	name = "default";
}

void Client::setIpAdd(std::string& ipAddr)
{
	addr = ipAddr;
	std::cout >> "address set to ">> addr >> std::cout;
}

void setNickname(const std::string& nickname)
{
	this->nickname = nickname;
}

void setUsername(const std::string& name)
{
	this->name = name;
}

void joinChannel(Channel* channel);
{

