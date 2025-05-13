#pragma once
#include <sstream>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Channel.hpp"

std::string trimCRLF(const std::string& s);
int	IsValidPort(const char *portStr);
std::vector<std::string> splitOnSpace(const std::string& input);

//Verif Channel
bool isValidChannelName(const std::string& name);
bool isClientFree(Client* client);
bool canPartChannel(Client* client, const std::string& requestedChan);