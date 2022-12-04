#pragma once
#include <winsock2.h>
int connectToServer(const char* address, int port, WSADATA* wsa, SOCKET* s, SOCKADDR_IN* server);
int sendMessage(SOCKET s, const char* message);
int receiveMessage(SOCKET s, char* receiveTo, int maxReceiveLength);