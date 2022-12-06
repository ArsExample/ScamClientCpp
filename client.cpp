#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <winsock2.h>
#include <string>

#include "Client.h"
#include "Online.h"
#include "uniID.h"
#include "Commands.h"

using namespace std;

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET server;
	SOCKADDR_IN serverAddress;

	char receiveBuffer[2048];

	//Connect to remote server
	if (connectToServer("127.0.0.1", 26780, &wsa, &server, &serverAddress))
	{
		int userID;
		getID(server, &userID);
		puts(("Client ID: " + to_string(userID)).c_str());

		while (true)
		{
			char receiveBuffer[2048];

			if (receiveMessage(server, receiveBuffer, sizeof receiveBuffer) == 0) { // получаем ответ от сервера
				puts("Failed get data from server"); // C$id клиента$id комманды$успех
			}
			else
			{
				string data = string(receiveBuffer);
				puts(("DATA FROM SERVER: " + data).c_str());
				parseCmd(server, userID, data);
			}
		}
	}
	else
	{
		puts("Failed");
	}

	closesocket(server);
	WSACleanup();
}