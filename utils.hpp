#pragma once
#include <sstream>
#include <string>
#include <vector>

std::string trimCRLF(const std::string& s);
int	IsValidPort(const char *portStr);
std::vector<std::string> splitOnSpace(const std::string& input);
