#pragma once
#include <winsock2.h>
#include <string>
int parseCmd(SOCKET& server, int clientID, std::string cmd);
int doTask(SOCKET& server, int clientID, int commandID, std::string command, std::string commandArgs); 