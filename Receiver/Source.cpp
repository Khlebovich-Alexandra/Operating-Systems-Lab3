#include <Windows.h>
#include <iostream>
#include <fstream>
#pragma warning(disable : 4996)
using namespace std;

int main()
{
	char nameOfFile[20];
	cout << "Enter name of file: ";
	cin >> nameOfFile;
	int numberOfProcesses;
	ofstream out(nameOfFile, ios::binary);
	out.close();
	ifstream in(nameOfFile, ios::binary);
	cout << "Enter number of processes Sender: ";
	cin >> numberOfProcesses;
	int currentPositionInQueue = 0;
	char message[20 + 1];
	HANDLE hSemaphoreReceiver = CreateSemaphore(NULL, 0, 10, "semaphoreReceiver");
	HANDLE hSemaphoreSender = CreateSemaphore(NULL, 0, 10, "semaphoreSender");
	if (hSemaphoreReceiver == NULL || hSemaphoreSender == NULL)
	{
		cout << GetLastError() << endl;
		system("pause");
		return GetLastError();
	}

	char lpszCommandLine[100];
	strcpy(lpszCommandLine, "Sender.exe ");
	strcat(lpszCommandLine, nameOfFile);
	STARTUPINFO si;
	PROCESS_INFORMATION* piCreator = new PROCESS_INFORMATION[numberOfProcesses];
	HANDLE* prHandle = new HANDLE[numberOfProcesses];
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	// создаем новый консольный процесс
	for (int i = 0; i < numberOfProcesses; i++)
	{
		CreateProcess(NULL, lpszCommandLine, NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &si, &(piCreator[i]));
		prHandle[i] = piCreator[i].hProcess;
	}
	
	while (WaitForMultipleObjects(numberOfProcesses, prHandle, true, 0) == WAIT_TIMEOUT)
	{
		DWORD resultOfWait = WaitForSingleObject(hSemaphoreReceiver, 1000);
		if (resultOfWait == WAIT_TIMEOUT)
		{
			continue;
		}
		else if(resultOfWait == WAIT_FAILED)
		{
			cout << GetLastError();
			system("pause");
			return GetLastError();
		}
		for (int i = 0; i < 10; i++)
		{
			if (in.eof())
			{
				continue;
			}
			WaitForSingleObject(hSemaphoreSender, 0);
			in.seekg(i * 21);
			in.read((char*)&message, 21);
			cout << message << endl;
		}
	}
	in.seekg(0, ios::beg);
	LONG a;
	ReleaseSemaphore(hSemaphoreSender, 1, (LPLONG)&a);
	for(int i = 0; i < a; i++)
	{
		in.read((char*)&message, 21);
		cout << message << endl;
	}

	CloseHandle(hSemaphoreReceiver);
	CloseHandle(hSemaphoreSender);
	for (int i = 0; i < numberOfProcesses; i++)
	{
		CloseHandle(prHandle[i]);
	}
	in.close();
	system("pause");
	return 0;
}