#include <fstream>
#include <iostream>
#include <conio.h>
#include <Windows.h>
using namespace std;

int main(int argc, char *argv[])
{
	std::ofstream out(argv[1], std::ios::binary);
	char message[20 + 1];
	if (!out) throw "Error";
	HANDLE hSemaphoreSender = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, "semaphoreSender");
	HANDLE hSemaphoreReceiver = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, "semaphoreReceiver");
	if (hSemaphoreSender == NULL || hSemaphoreReceiver == NULL)
	{
		return GetLastError();
	}
	LONG a = -1;
	while (true)
	{
		cout << "Send message or quit programm?" << endl;
		cout << "1 - send; 0 - quit" << endl;
		int tmp;
		cin >> tmp;
		if (tmp == 0)
		{
			break;
		}
		cout << "Enter message: ";
		cin >> message;
		bool flag = ReleaseSemaphore(hSemaphoreSender, 1, (LPLONG)&a);
		if (!flag)
		{
			return GetLastError();
		}
		out.seekp(a * 21, ios::beg);
		out.write(message, 21);
		out.flush();
		if (a == 9)
		{
			bool flag = ReleaseSemaphore(hSemaphoreReceiver, 1, NULL);
			if (!flag)
			{
				return GetLastError();
			}
		}
		Sleep(100);
	}
	out.close();
	CloseHandle(hSemaphoreSender);
	CloseHandle(hSemaphoreReceiver);
	system("pause");
	return 0;

}
