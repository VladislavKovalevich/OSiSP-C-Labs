#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#define COL 3
#define ROW 5
#define SIZE_BUFFER 255
#define SIZE_BUFFER_STRING 100
#define SIZE_BUFFER 260
#define PATH "D:/test.txt"

BOOL RegClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawTable(HWND, HDC);
int GetLongestString(int);
int GetDataForTable();
void CheckCellSize(HWND, HDC, TEXTMETRIC, LOGFONT);

extern char szClassName[] = "WndClass";
extern char szTitle[] = "Имя Окна";
static HWND IndexWindow;
static HINSTANCE hInst;
static char* bufferString[SIZE_BUFFER_STRING];

int WinWindth, WinHigh, maxLength, nextRowHigh;
LONG cellwidth = 0, cellhigh = 0, maxHigh = 0;
char* bufForTable[ROW*COL];
int maxHighArr[ROW];
int countStr = 0;
int max = 0;
RECT table[ROW][COL];
RECT rectWindow, cellRect;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int nCmdShow)
{
	//DWORD pid = GetCurrentProcessId();
	//MessageBox(hInstance, (char*)pid, "Caption",MB_OK);
	static MSG msg;
	hInst = hInstance;

	countStr = GetDataForTable();
	maxLength = GetLongestString(countStr);

	if (!RegClass(WndProc, szClassName, COLOR_WINDOW))
	{
		return FALSE;
	}

	IndexWindow = CreateWindow(szClassName, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 500, 300, 800, 700, 0, 0, hInst, NULL);

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

BOOL RegClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = Proc;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(brBackground + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szName;

	return (RegisterClass(&wc) != 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TEXTMETRIC tm = {NULL};
	LONG maxHighCell = 0;
	boolean flag = FALSE;
	LOGFONT lf = {18, 10, 0, 0, 0, TRUE};
	HFONT hft = CreateFontIndirect(&lf);
	HFONT OldFt;

	switch (msg)
	{
		case WM_CREATE: 
		{
			GetClientRect(hwnd, &rectWindow);
			break;
		}

		case WM_PAINT: 
		{
			GetClientRect(hwnd, &rectWindow);

			WinWindth = rectWindow.right;
			WinHigh = rectWindow.bottom;

			hdc = BeginPaint(hwnd, &ps);
			OldFt = (HFONT)SelectObject(hdc, hft);

			cellwidth = (LONG)((double)WinWindth / (double)COL);
			cellhigh = (LONG)((double)WinHigh / (double)ROW);
			
			GetTextMetrics(hdc, &tm);

			DrawTable(hwnd, hdc);
			CheckCellSize(hwnd, hdc, tm, lf);

			ValidateRect(hwnd, &rectWindow);
			EndPaint(hwnd, &ps);
			break;
		}

		case WM_SIZE:
		{
			
			GetClientRect(hwnd, &rectWindow);

			WinWindth = rectWindow.right;
			WinHigh = rectWindow.bottom;

			break;
		}

		case WM_DESTROY:
		{
			
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void DrawTable(HWND hwnd, HDC hdc)
{
	int strCounter = 0;
	int high = 0, maxHigh = 0;
	int ArrHigh[ROW];

	nextRowHigh = 0;

	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (i == 0)
			{
				table[i][j].top = 0;
			}
			else
			{
				table[i][j].top = nextRowHigh + i * 2;
				
			}

			if (j == 0)
			{
				table[i][j].left = 0;
			}
			else
			{
				table[i][j].left = table[i][j - 1].left + (int)cellwidth + 2;
			}


			table[i][j].bottom = table[i][j].top + 90;
			
			table[i][j].right = table[i][j].left + cellwidth;

			if (strCounter < countStr) 
			{
				high = DrawText(hdc, bufForTable[strCounter], -1, &table[i][j], DT_WORDBREAK);
				strCounter++;
			}
			else 
			{
				high = DrawText(hdc, bufForTable[countStr - 3], -1, &table[i][j], DT_WORDBREAK);
			}

			if (maxHigh == 0 || high > maxHigh) 
			{
				maxHigh = high;
			}

		}
		nextRowHigh += maxHigh + i*2;
		ArrHigh[i] = nextRowHigh;
		maxHigh = 0;
	}

	MoveToEx(hdc, 0, 0, NULL);
	LineTo(hdc, cellwidth*COL, 0);

	for (int i = 0; i < ROW; i++) 
	{
		MoveToEx(hdc, 0, ArrHigh[i], NULL);
		LineTo(hdc, cellwidth * COL, ArrHigh[i]);
	}

	for (int j = 0; j <= COL; j++) 
	{
		MoveToEx(hdc, cellwidth * j, 0, NULL);
		LineTo(hdc, cellwidth * j, nextRowHigh);
	}

}


int GetLongestString(int ctStr)
{
	int maxlen = 0;

	for (int i = 0; i < ctStr; i++)
	{
		char * string = (char*)calloc(256,sizeof(char));
		string = bufForTable[i];
		
		int c = 0;
		for (int j = 0; j < strlen(bufForTable[i]); j++)
		{
			if (bufForTable[i][j] == ' ')
			{
				if ((maxlen == 0) || (c > maxlen))
					maxlen = c;
				c = 0;
			}
			c++;
		}
	}

	return maxlen + 3;
}

int GetDataForTable() 
{
	HANDLE DataFilehandle = CreateFile(PATH,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	OVERLAPPED olf = { 0 }; 
	LARGE_INTEGER li = { 0 };

	li.QuadPart = 0;
	olf.Offset = li.LowPart;
	olf.OffsetHigh = li.HighPart;

	LPSTR buffer = (CHAR*)calloc(SIZE_BUFFER + 1, sizeof(CHAR));
	DWORD i = 0;
	DWORD iNumRead = 1;

	while (&iNumRead != 0 || (olf.Internal != -1 && !GetLastError))
	{
		if (!ReadFile(DataFilehandle, buffer, SIZE_BUFFER, &iNumRead, &olf)) 
		{
			break;
		}

		LPSTR currStr = strtok(buffer, "|");

		while (currStr != NULL) 
		{
			bufForTable[i++] = currStr;
			currStr = strtok(NULL, "|");
		}

		olf.Offset += iNumRead;
	}

	CloseHandle(DataFilehandle);

	return i;
}

void CheckCellSize(HWND hwnd, HDC hdc, TEXTMETRIC tm, LOGFONT lf)
{
	if (tm.tmAveCharWidth * (LONG)maxLength + 6 > cellwidth)
	{
		cellwidth = tm.tmAveCharWidth * (LONG)maxLength + 6;
		DrawTable(hwnd, hdc);
		SetWindowPos(hwnd, NULL, 0, 0, (tm.tmAveCharWidth * (LONG)maxLength + 6) * COL + 5 * COL, nextRowHigh + 42, SWP_NOMOVE);
		DrawTable(hwnd, hdc);
	}
	else 
	{
		DrawTable(hwnd, hdc);
		SetWindowPos(hwnd, NULL, 0, 0, cellwidth*COL+3*COL, nextRowHigh + 42, SWP_NOMOVE);
		DrawTable(hwnd, hdc);
	}
}
