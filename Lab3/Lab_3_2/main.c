#include <windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <stdlib.h>

#define DLL_PATH "D:\\Учёба\\Лабораторные\\Семестр 5\\ОСиСП\\Lab_3\\Debug\\DllLibarary.dll"
#define FUNC_NAME "GetNumber23"
#define FUNC_REPLACE_NAME "ReplaceStringInMem"
#define FUNC_REPLACE_WITH_STRUCT "ReaplaceStringWithStruct"
#define EXE_NAME "Lab2_CreateTable.exe"

#define BUFFER_SIZE 254

int main()
{
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	DWORD pid = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnap)
	{
		printf("Error: can't create Snapshot");
		return 1;
	}
	
	if (Process32First(hSnap, &pe)) 
	{
		while (Process32Next(hSnap, &pe))
		{
			if (!strcmp(pe.szExeFile, EXE_NAME)) 
			{
				pid = pe.th32ProcessID;
				printf("pid :%d,\t\tname process:%s\n", pe.th32ProcessID, pe.szExeFile);
				break;
			}
		}
		CloseHandle(hSnap);
	}

	printf("0 - static; 1 - dynamic; 2 - in other process\n");
	char method = getchar();

	int arg = 13;

	const char* oldStr = "dqed";
	const char* newStr = "vlad";

	switch (method)
	{
	case '0':
	{
		printf("static method(.lib)\n");
		//printf("%d\n", GetNumber23(arg));

		if (ReplaceStringInMem(oldStr, newStr, pid))
		{
			printf("end of procedure...\n");
		}
		break;
	}
	case '1':
	{
		printf("dynamic method(.dll)\n");
		HMODULE hDll = LoadLibrary(DLL_PATH);

		if (!hDll)
		{
			printf("Error\n");
			break;
		}

		//int(*GetN)() = NULL;
		BOOL(*ReplaceString)() = NULL;

		//(FARPROC*)GetN = GetProcAddress(hDll, FUNC_NAME);
		(FARPROC*)ReplaceString = GetProcAddress(hDll, FUNC_REPLACE_NAME);

		if (ReplaceString)
		{
			//int val = GetN(arg);
			if (ReplaceString(oldStr, newStr, pid))
			{
				printf("end of procedure...\n");
			}
		}

		FreeLibrary(hDll);
		break;
	}
	case '2':
	{
		DWORD pid2 = GetCurrentProcessId();
		HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid2);

		if (proc == NULL)
		{
			printf("Error: cannot open processs\n");
			break;
		}

		HMODULE hModule = LoadLibrary(TEXT(DLL_PATH));
		LPVOID GetNum = (LPVOID)GetProcAddress(hModule, FUNC_REPLACE_WITH_STRUCT);

		if (GetNum == NULL)
		{
			printf("Error: cannot get procedure\n");
			break;
		}

		struct PARAM
		{
			const char* newStr;// = (char*)calloc(BUFF_SIZE + 1, sizeof(char));
			const char* oldStr;// = (char*)calloc(BUFF_SIZE + 1, sizeof(char));
			DWORD pid;
		}param;

		param.newStr = newStr;
		param.oldStr = oldStr;
		param.pid = pid;

		LPVOID alloc = VirtualAllocEx(proc, NULL, sizeof(param), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		if (alloc == NULL)
		{
			printf("Error: cannot alloc memory\n");
			break;
		}

		if (!WriteProcessMemory(proc, alloc, &param, sizeof(param), NULL))
		{
			printf("Error: cannot write in processs\n");
			break;
		}

		HANDLE rmThread = CreateRemoteThread(proc, 0, 0, (LPTHREAD_START_ROUTINE)GetNum, (LPVOID)alloc, 0, 0);

		if (rmThread == NULL)
		{
			printf("Error: cannot create process\n");
			break;
		}

		CloseHandle(proc);
		WaitForSingleObject(rmThread, INFINITE);

		printf("Func is processed\n");
		CloseHandle(rmThread);
		break;
	}

	default:
		break;
	}

	system("pause");
	return 0;
}