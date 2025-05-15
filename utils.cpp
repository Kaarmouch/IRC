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

std::string str_toupper(std::string str)
{
	for (std::string::size_type i = 0; i < str.size(); ++i)
		str[i] = std::toupper(static_cast<unsigned char>(str[i]));
	return str;
}

std::vector<std::string> splitByCRLF(const std::string& input)
{
	std::vector<std::string> result;
	size_t start = 0, end;

	while ((end = input.find("\r\n", start)) != std::string::npos) 
	{
		std::string part = input.substr(start, end - start);
		part = trim(part);
		if (!part.empty())
		{
			result.push_back(part);
			std::cout <<"added " + part << std::endl;
		}
		start = end + 2; // skip over "\r\n"
	}
	if (start < input.length()) 
	{
		std::string part = trim(input.substr(start));
		std::cout << "[rest: " << part << "]" << std::endl;
		if (!part.empty())
			result.push_back(part);
	}
	return result;
}

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) return "";
	size_t last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, (last - first + 1));
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

