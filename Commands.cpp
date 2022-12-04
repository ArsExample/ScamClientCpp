#include <string>
#include <vector>
#include <regex>
#include <thread>


#include "Online.h"

using namespace std;


int doTask(SOCKET& server, int clientID, int commandID, string command, string commandArgs) // поток для выполнения команды, передаем server - сам сервер и другие параметры
{
	string msg;

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
		try
		{
			keybd_event(VK_MENU, NULL, NULL, NULL);
			keybd_event(VK_TAB, NULL, NULL, NULL);
			keybd_event(VK_MENU, NULL, KEYEVENTF_KEYUP, NULL);
			keybd_event(VK_TAB, NULL, KEYEVENTF_KEYUP, NULL);
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
}

vector<string> split(const string& input, const string& regex) {
	// passing -1 as the submatch index parameter performs splitting
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

	data = cmd.substr(0, data.length() - 2);
	args = split(data, "\\$");

	commandID = atoi(args[0].c_str());
	command = args[1];
	commandArgs = args[2];

	command = upperToLower(command);
	commandArgs = upperToLower(commandArgs);

	thread t1(doTask, ref(server), clientID, commandID, command, commandArgs);
	t1.join();

	return 1;
}