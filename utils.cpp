#include "utils.hpp"

std::vector<std::string> splitOnSpace(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string word;

    while (stream >> word) {
        tokens.push_back(word);
    }

    return tokens;
}

std::string trimCRLF(const std::string& s) {
    std::string out = s;
    while (!out.empty() && (out[out.size() - 1] == '\n' || out[out.size() - 1] == '\r')) {
        out.erase(out.size() - 1, 1);
    }
    return out;
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
    if (client->getChanOn() != "No channel")
    {
        std::cout << "[Channel Check] Client already in channel: " << client->getChanOn() << std::endl;
        return false;
    }
    return true;
}

bool canPartChannel(Client* client, const std::string& requestedChan)
{
    std::string currentChan = client->getChanOn();

    if (currentChan == "No channel")
    {
        std::cout << "[Part Check] Client not in any channel.\n";
        client->sendMessage("You are not in any channel.");
        return false;
    }
    if (requestedChan != currentChan)
    {
        std::cout << "[Part Check] Client tried to part " << requestedChan << " but is in " << currentChan << ".\n";
        client->sendMessage("You are not in " + requestedChan + ".");
        return false;
    }
    return true;
}

