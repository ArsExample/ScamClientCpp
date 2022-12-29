#include "Commands.h"
#include "Online.h"

using namespace std;

/* enum Всех команд, которые могут быть
* надо для свича
*/
typedef enum
{
	CMD_INVALID = -1,
	CMD_MYSTIFY,
	CMD_SWITCH,
	CMD_PRESS,
	CMD_SETMOUSE,
	CMD_MOVEMOUSE,
	CMD_COMMANDLINE,
	CMD_CLOSE,
	CMD_LANG,
	CMD_HIDE,
	CMD_MOUSECLICK,
	CMD_PROCESSKILL
} Commands;

/* Структура для сохранения соответствия между командой и строкой */
typedef struct tagCommand
{
	string command;  //строка
	Commands cmd;    //штука switch по которой делается (switch по строкам отсутсотвует((( )
} Command;

static void sendCommandResult(SOCKET& server, int clientID, int commandID, bool success)
{
	string msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + (success ? "success" : "fail"); // формируем ответ
	sendMessage(server, msg.c_str());
}

int doTask(SOCKET& server, int clientID, int commandID, string command, string commandArgs) // поток для выполнения команды, передаем server - сам сервер и другие параметры
{
	bool failed = false;
	string msg;
	Command current = {"", CMD_INVALID};
	vector<string> pos; //for setmouse
	static Command Commands[] =
	{
	  {"mystify", CMD_MYSTIFY},
	  {"switch", CMD_SWITCH},
	  {"press", CMD_PRESS},
	  {"setmouse", CMD_SETMOUSE},
	  {"move", CMD_MOVEMOUSE},
	  {"cmd", CMD_COMMANDLINE},
	  {"close", CMD_CLOSE},
	  {"lang", CMD_LANG},
	  {"hide", CMD_HIDE},
	  {"click", CMD_MOUSECLICK},
	  {"kill", CMD_PROCESSKILL},
	};

	printf("started doing task: %s, %s...", command.c_str(), commandArgs.c_str());

	for (int i = 0; i < sizeof(Commands) / sizeof(Commands[0]); i++)
		if (command == Commands[i].command)
		{
			current = Commands[i];
			break;
		}

	switch (current.cmd)
	{
	case CMD_MYSTIFY:
	{
		try
		{
			system("mystify.scr -a");
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_SWITCH:
	{
		puts("started doing switch command...");
		try
		{
			keybd_event(VK_MENU, NULL, NULL, NULL);

			if (commandArgs == "none")
			{
				keybd_event(VK_TAB, NULL, NULL, NULL);
				keybd_event(VK_TAB, NULL, KEYEVENTF_KEYUP, NULL);
				Sleep(100);
			}
			else {
				for (int i = 0; i < stoi(commandArgs); i++)
				{
					keybd_event(VK_TAB, NULL, NULL, NULL);
					keybd_event(VK_TAB, NULL, KEYEVENTF_KEYUP, NULL);
					puts("switch");
					Sleep(100);
				}
			}
			keybd_event(VK_MENU, NULL, KEYEVENTF_KEYUP, NULL);
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_PRESS:
	{
		char key = commandArgs.c_str()[0];
		try
		{
			keybd_event(VkKeyScan(key) & 0xff, NULL, NULL, NULL);
			keybd_event(VkKeyScan(key) & 0xff, NULL, KEYEVENTF_KEYUP, NULL);
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_SETMOUSE:
	{
		try
		{
			try
			{
				pos = split(commandArgs, ",");
			}
			catch (const std::exception&)
			{
				msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "unknown_command";
				sendMessage(server, msg.c_str());
				return 0;
			}
			SetCursorPos(stoi(pos[0]), stoi(pos[1]));
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_MOVEMOUSE:
	{
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
				sendMessage(server, msg.c_str());
				return 0;
			}
			GetCursorPos(&p);
			SetCursorPos(p.x + stoi(pos[0]), p.y + stoi(pos[1]));
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_COMMANDLINE:
	{
		try
		{
			system(commandArgs.c_str());
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_CLOSE:
	{
		try
		{
			HWND hWnd;
			hWnd = GetForegroundWindow();
			DWORD process_ID = 0;
			GetWindowThreadProcessId(hWnd, &process_ID);
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_ID);
			TerminateProcess(hProcess, 9);
			CloseHandle(hProcess);
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_LANG:
	{
		try
		{
			PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, 2, 0);
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_HIDE:
	{
		try
		{
			keybd_event(VK_LWIN, NULL, NULL, NULL);
			keybd_event(0x44, NULL, NULL, NULL);
			keybd_event(VK_LWIN, NULL, KEYEVENTF_KEYUP, NULL);
			keybd_event(0x44, NULL, KEYEVENTF_KEYUP, NULL);
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_MOUSECLICK:
	{
		POINT p;
		puts("started click");
		try
		{
			GetCursorPos(&p);
			if (commandArgs == "left")
			{
				puts("left");
				mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
			}
			else if (commandArgs == "right")
			{
				mouse_event(MOUSEEVENTF_RIGHTDOWN, p.x, p.y, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
			}
			else 
			{
				puts("else");
				msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "unknown_command";
				sendMessage(server, msg.c_str());
				return 0;
			}
		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_PROCESSKILL:
	{
		string name;
		try
		{
			name = commandArgs;
			PROCESSENTRY32 entry;
			entry.dwSize = sizeof(PROCESSENTRY32);

			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

			if (Process32First(snapshot, &entry) == TRUE)
			{
				while (Process32Next(snapshot, &entry) == TRUE)
				{
					_bstr_t b(entry.szExeFile);
					const char* c = b;

					if (_stricmp(c, name.c_str()) == 0)
					{
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

						TerminateProcess(hProcess, 9);

						puts("123");

						CloseHandle(hProcess);
					}
				}
			}
			CloseHandle(snapshot);

		}
		catch (const exception&)
		{
			failed = true;
		}
	}
	break;
	case CMD_INVALID:
	default:
		msg = "C$" + to_string(clientID) + "$" + to_string(commandID) + "$" + "unknown_command";
		sendMessage(server, msg.c_str());
	}

	sendCommandResult(server, clientID, commandID, !failed);
	printf("Task %s, %s done: %s\n", command.c_str(), commandArgs.c_str(), (failed ? "failed" : "success"));
	return 1;
}

int doSystemTask(SOCKET& server, string command, string commandArgs)
{
	if (command == "shutdown")
	{
		puts("Server shutdown, closing app");
		closesocket(server); // todo: reconnect to server in every ~30sek while not connect
		WSACleanup();
		abort();
	}
	else if (command == "disconnect")
	{
		puts("Disconnect, closing app");
		closesocket(server);
		WSACleanup();
		abort();
	}

	return 1;
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

	for (int i = 0; i < str.length(); i++)
		result += tolower(str[i]);

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
	else if (!command.empty())
		throw std::runtime_error("[COMMANDS.cpp] Unknown command: " + command);

	return 1;
}