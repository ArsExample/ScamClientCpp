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

int getID(SOCKET server, int *userID)  // server - ������, ����� ������� ��� ����� �����������, userID - ����������, � ������� ��������� ���������
{	
	int uniUserID;
	if (isFile("userUniID.txt")) // ��������� ������������� ����� userUniID.txt, ���� ���� ����, ��:
	{
		char data[32];
		ifstream fin("userUniID.txt"); // ��������� ����

		fin.getline(data, 32); // ������
		fin.close();
		uniUserID = atoi(data);

		string str_uniID = to_string(uniUserID);
		string command = "Client$" + str_uniID;

		if (sendMessage(server, command.c_str()) == 0) // �������� �������, ��� �� �����
		{
			puts("Failed sending login message");
		}
	}
	else { // ���� ����� ���:
		puts("Attempting to create id");
		uniUserID = randint();

		string command = "Client$-" + to_string(uniUserID); // �������� ������� ��������������� id �� �������� (- ����� $ ��� �����������)
		if (sendMessage(server, command.c_str()) == 0) {
			puts("Failed sending login message");
			return 0;
		}

		char receiveBuffer[2048];

		if (receiveMessage(server, receiveBuffer, sizeof receiveBuffer) == 0) { // �������� ����� �� �������
			puts("Failed get data from server");
			return 0;
		}

		string data = string(receiveBuffer);
		if (contains(data, "LOGIN$CONNECT") == 0) // ���� ����� �� �������� ����� (�� ���������� ������������������), ��:
		{
			puts("not success");
			while (contains(data, "LOGIN$CONNECT") == 0) // ���� �� ��������� ������������������
			{
				uniUserID = randint();

				string command = "Client$-" + to_string(uniUserID); // �������� ������� ������ � ����� id
				if (sendMessage(server, command.c_str()) == 0) {
					puts("Failed sending login message");
					return 0;
				}

				char receiveBuffer[2048];

				if (receiveMessage(server, receiveBuffer, sizeof receiveBuffer) == 0) { // �������� �����
					puts("Failed get data from server");
					return 0;
				}

				data = string(receiveBuffer); // �������������� �����
			}
		}
		puts("Trying to create file");
		ofstream fout("userUniID.txt"); // ������� ����
		fout << to_string(uniUserID); // ��������� � ���� id
		fout.close();
	}
	*userID = uniUserID; // ���������� ��������� � ����������
	return 0;
}