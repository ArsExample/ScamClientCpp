#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "Online.h"

int connectToServer(const char* address, int port, WSADATA* wsa, SOCKET* s, SOCKADDR_IN* server)
{
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 0;
	}

	printf("Initialised.\n");

	if ((*s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		return 0;
	}

	printf("Socket created.\n");
	server->sin_addr.s_addr = inet_addr(address);
	server->sin_family = AF_INET;
	server->sin_port = htons(port);

	if (connect(*s, (struct sockaddr*)server, sizeof(*server)) < 0)
	{
		puts("Connection error");
		return 0;
	}
	return 1;
}

int sendMessage(SOCKET s, const char* message)
{
	char* toSend;
	if (message[strlen(message) - 1] != '\n')
	{
		toSend = new char[strlen(message) + 2];
		strncpy(toSend, message, strlen(message));
		toSend[strlen(message)] = '\n';
		toSend[strlen(message) + 1] = '\0';
	}
	else
	{
		toSend = new char[strlen(message) + 1];
		strncpy(toSend, message, strlen(message));
		toSend[strlen(message)] = '\0';
	}

	if (send(s, toSend, strlen(toSend), 0) < 0)
		return 0;

	return 1;
}

int receiveMessage(SOCKET s, char* receiveTo, int maxReceiveLength)
{
	int recv_size;
	if ((recv_size = recv(s, receiveTo, maxReceiveLength, 0)) == SOCKET_ERROR)
		return 0;

	receiveTo[recv_size] = '\0';
	return 1;
}