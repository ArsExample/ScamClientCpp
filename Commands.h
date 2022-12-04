#pragma once

#include <winsock2.h>
#include <string>

int parseCmd(SOCKET& server, int clientID, std::string cmd);