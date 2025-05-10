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

