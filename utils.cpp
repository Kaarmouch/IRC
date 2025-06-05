#include "utils.hpp"
#include <iostream>

std::vector<std::string> splitOnSpace(const std::string& input) 
{
	std::vector<std::string> tokens;
	std::istringstream stream(input);
	std::string word;

	while (stream >> word) {
        tokens.push_back(word);
	}
	return tokens;
}

std::string get_pass(const std::string& input)
{
	std::vector<std::string> vec = splitOnSpace(input);
	if (vec[0] == "PASS" && (vec.size() >= 2 && !(vec[1].empty())))
		return vec[1];
	return vec[0];
}

std::string str_toupper(std::string str)
{
	for (std::string::size_type i = 0; i < str.size(); ++i)
		str[i] = std::toupper(static_cast<unsigned char>(str[i]));
	return str;
}

std::string getReason(const std::vector<std::string>& words)
{
	std::string r;
	if (words.size() < 4 || words[3].empty())
		return "No reason";
	for (size_t i = 3;i < words.size(); i++)
	{
		if (i == 4 && words[i][0] == ':')
			r += words[i].substr(1);
		else
			r += words[i];
		if ((i + 1) != words.size())
			r += " ";
	}
	return r;
}

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) return "";
	size_t last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, (last - first + 1));
}

bool isPositiveInt(const std::string& str, int& result)
{
    if (str.empty())
        return false;

    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isdigit(*it))
            return false;
    }
    std::istringstream iss(str);
    iss >> result;
    return !iss.fail() && result > 0;
}

int	IsValidPort(const char *portStr)
{
	std::string	str(portStr);
	int	port;

	std::istringstream iss(str);
	iss >> port;
	if (iss.fail() || !iss.eof())
		return 0;
	// Vérifie la plage autorisée
	if (port < 1024 || port > 65535)
		return 0;
	return port;
}


bool isValidChannelName(const std::string& name)
{
    if (name.empty() || name[0] != '#') 
    {
        std::cout << "[Channel Check] Invalid: must start with '#'\n";
        return false;
    }
    if (name.length() > 50) 
    {
        std::cout << "[Channel Check] Invalid: too long\n";
        return false;
    }
    for (size_t i = 0; i < name.length(); ++i)
    {
        char c = name[i];
        if (c == ' ' || c == ',' || c == '\a' || c == '\0' || c == ':')
            return false;
    }
    return true;
}

bool isClientFree(Client* client)
{
    if (client->getChanOn() != NULL)
    {
        std::cout << "[Channel Check] Client already in channel: " << client->getChanOn()->getName() << std::endl;
        return false;
    }
    return true;
}

bool canPartChannel(Client* client, const std::string& requestedChan)
{
    Channel* chan = client->getChanOn();

    if (chan == NULL)
    {
        std::cout << "[Part Check] Client not in any channel.\n";
        client->sendMessage("You are not in any channel.");
        return false;
    }
    if (chan->getName() != requestedChan)
    {
        std::cout << "[Part Check] Client tried to part " << requestedChan << " but is in " << chan->getName() << ".\n";
        client->sendMessage("You are not in " + requestedChan + ".");
        return false;
    }
    return true;
}