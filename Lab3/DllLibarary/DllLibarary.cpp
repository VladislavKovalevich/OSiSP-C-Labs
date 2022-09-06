// DllLibarary.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <malloc.h>
#include <TLhelp32.h>
#include <string>
#include <iostream>
#include <vector>

#define MYLIBAPI __declspec(dllexport)

#include "DllLibarary.h"


int GetNumber23(int arg1)
{
	printf("is return");
	return 11 * arg1;
}

#define BUFF_SIZE 254
#define BUFFER_SIZE 89120000

BOOL ReplaceStringInMem(const char * oldStr, const char * newStr, DWORD pid)
{
	if (strlen(oldStr) != strlen(newStr)) 
	{ 
		printf("Error: invalide string");
		return false; 
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (hProc == NULL) 
	{
		printf("Error : open process");
		return false; 
	}

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	MEMORY_BASIC_INFORMATION info;
	std::vector<char> chunk;
	char* p = 0;
	while (p < si.lpMaximumApplicationAddress)
	{

		if (VirtualQueryEx(hProc, p, &info, sizeof(info)) == sizeof(info))
		{

			if (info.State == MEM_COMMIT && info.AllocationProtect == PAGE_READWRITE)
			{
				p = (char*)info.BaseAddress;

				chunk.resize(info.RegionSize);
				SIZE_T bytesRead;

				try {
					if (ReadProcessMemory(hProc, p, &chunk[0], info.RegionSize, &bytesRead))
					{
						for (size_t i = 0; i < (bytesRead - strlen(oldStr)); ++i)
						{
							if (memcmp(oldStr, &chunk[i], strlen(oldStr)) == 0)
							{
								/*char* ref = (char*)p + i;
								for (int j = 0; j < strlen(newStr); j++) {
									ref[j] = newStr[j];
								}
								ref[strlen(newStr)] = 0;*/
								if(WriteProcessMemory(hProc, p + i, newStr, strlen(newStr), &bytesRead)) 
								{
									printf("is replaced\n");
								}
							}
						}
					}
				}
				catch (std::bad_alloc& e) {

				}
			}
			p += info.RegionSize;
		}
	}

	CloseHandle(hProc);
	return true;
}

BOOL ReaplaceStringWithStruct(PVOID structParam)
{
	struct PARAMS
	{
		const char* newStr = (char*)calloc(BUFF_SIZE + 1, sizeof(char));
		const char* oldStr = (char*)calloc(BUFF_SIZE + 1, sizeof(char));
		DWORD pid;
	};

	PARAMS *pParams = (PARAMS *)structParam;

	const char *oldstr = (*pParams).oldStr;
	const char *newstr = (*pParams).newStr;
	DWORD pid = (*pParams).pid;

	return ReplaceStringInMem(oldstr, newstr, pid);
}

