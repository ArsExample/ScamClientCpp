#pragma once

#include <winsock2.h>
#include <string>
#include <vector>

int parseCmd(SOCKET& server, int clientID, std::string cmd);
std::vector<std::string> split(const std::string& input, const std::string& regex);