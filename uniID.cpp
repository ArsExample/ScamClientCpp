#include <string>
#include <fstream>

#include "Online.h"

using namespace std;

int isFile(string fname)
{
	ifstream fin(fname);
	int res = fin.is_open();
	fin.close();
	return res;
}

int randint()
{
	return rand() % 32767 + 1;
}

int contains(string str, string substr)
{
	return str.find(substr) != std::string::npos;
}

int getID(SOCKET server, int *userID)  // server - сервер, через который все будет происходить, userID - переменная, в которую запишется результат
{	
	int uniUserID;
	if (isFile("userUniID.txt")) // проверяем существование файла userUniID.txt, если есть акой, то:
	{
		char data[32];
		ifstream fin("userUniID.txt"); // открываем файл

		fin.getline(data, 32); // читаем
		fin.close();
		uniUserID = atoi(data);

		string str_uniID = to_string(uniUserID);
		string command = "Client$" + str_uniID;

		if (sendMessage(server, command.c_str()) == 0) // посылаем серверу, что мы гении
		{
			puts("Failed sending login message");
		}
	}
	else { // если файла нет:
		puts("Attempting to create id");
		uniUserID = randint();

		string command = "Client$-" + to_string(uniUserID); // посылаем серверу сгенерированный id на проверку (- после $ для регистрации)
		if (sendMessage(server, command.c_str()) == 0) {
			puts("Failed sending login message");
			return 0;
		}

		char receiveBuffer[2048];

		if (receiveMessage(server, receiveBuffer, sizeof receiveBuffer) == 0) { // получаем ответ от сервера
			puts("Failed get data from server");
			return 0;
		}

		string data = string(receiveBuffer);
		if (contains(data, "LOGIN$CONNECT") == 0) // если ответ не содержит успех (не получилось зарегистрироваться), то:
		{
			puts("not success");
			while (contains(data, "LOGIN$CONNECT") == 0) // пока не получится зарегистрироваться
			{
				uniUserID = randint();

				string command = "Client$-" + to_string(uniUserID); // посылаем серверу кманду о новом id
				if (sendMessage(server, command.c_str()) == 0) {
					puts("Failed sending login message");
					return 0;
				}

				char receiveBuffer[2048];

				if (receiveMessage(server, receiveBuffer, sizeof receiveBuffer) == 0) { // получаем ответ
					puts("Failed get data from server");
					return 0;
				}

				data = string(receiveBuffer); // расшифровываем ответ
			}
		}
		puts("Trying to create file");
		ofstream fout("userUniID.txt"); // создаем файл
		fout << to_string(uniUserID); // записывам в него id
		fout.close();
	}
	*userID = uniUserID; // записываем результат в переменную
	return 0;
}