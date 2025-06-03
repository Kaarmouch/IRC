#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

#include "Channel.hpp"
#include "Client.hpp"

class Client;

std::string get_pass(const std::string& input);
std::string str_toupper(std::string s);
std::string trim(const std::string& s);
int	IsValidPort(const char *portStr);
std::vector<std::string> splitOnSpace(const std::string& input);
bool isPositiveInt(const std::string& str, int& result);



//Verif Channel
std::string getReason(const std::vector<std::string>& words);
bool isValidChannelName(const std::string& name);
bool isClientFree(Client* client);
bool canPartChannel(Client* client, const std::string& requestedChan);
