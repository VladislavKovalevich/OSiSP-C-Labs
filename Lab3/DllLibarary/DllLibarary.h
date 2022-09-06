#pragma once
#include "stdafx.h"

#ifdef MYLIBAPI
#else
#define MYLIBAPI __declspec(dllimport)
#endif // MYLIBAPI


extern "C" MYLIBAPI int GetNumber23(int arg1);
extern "C" MYLIBAPI BOOL ReplaceStringInMem(const char* oldStr, const char* newStr, DWORD pid);
extern "C" MYLIBAPI BOOL ReaplaceStringWithStruct(PVOID stuctParam);