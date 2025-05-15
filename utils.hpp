#pragma once
#include <sstream>
#include <string>
#include <vector>

std::string trim(const std::string& s);
int	IsValidPort(const char *portStr);
std::vector<std::string> splitOnSpace(const std::string& input);
std::vector<std::string> splitByCRLF(const std::string& input);
