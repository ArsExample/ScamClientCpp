#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <regex>
#include <thread>
#include <stdio.h>
#include <Windows.h>
#include <comdef.h>

int parseCmd(SOCKET& server, int clientID, std::string cmd);
std::vector<std::string> split(const std::string& input, const std::string& regex);