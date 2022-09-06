#include<windows.h>
#include<string.h>
#include<stdlib.h>

BOOL RegClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void updateWin(int);

extern char szClassName[] = "WndClass";
extern char szTitle[] = "Имя Окна";

static HWND IndexWindow;
static HINSTANCE hInst;

HANDLE hBitmap;
BITMAP Bitmap;

static int spX, spY = 0;
static int WinWindth, WinHigh = 0;
static int time = 0;

int width, left, top, high;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int nCmdShow)
{
	static MSG msg;
	hInst = hInstance;

	if (!RegClass(WndProc, szClassName, COLOR_WINDOW))
	{
		return FALSE;
	}

	IndexWindow = CreateWindow(szClassName, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 500, 300, 1000, 700, 0, 0, hInst, NULL);
	
	hBitmap = LoadImage(NULL, "mario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SetTimer(IndexWindow, 1, 20, NULL);
	UpdateWindow(IndexWindow);
	
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(IndexWindow,1);
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
	RECT WinSize,prevWin;
	RECT rect;
	HDC hdc, objhdc;
	PAINTSTRUCT ps;
	HANDLE hOBitmap;

	switch (msg)
	{
		case WM_CREATE:
		{
			left = top = width = 120;
			high = 160;
			break;
		}

		case WM_PAINT:
		{
			GetClientRect(hwnd, &rect);

			hdc = BeginPaint(hwnd, &ps);
			objhdc = CreateCompatibleDC(hdc);
			hOBitmap = SelectObject(objhdc, hBitmap);
			//BitBlt(hdc,left, top, width, high, objhdc, 0, 0, SRCCOPY);
			StretchBlt(hdc, left, top, width, high, objhdc, 20, 0, 620, 690, SRCCOPY);
			SelectObject(objhdc, hOBitmap);

			ValidateRect(hwnd, &rect);
			EndPaint(hwnd, &ps);
			
			break;
		}

		case WM_SIZE: 
		{
			GetClientRect(hwnd, &rect);

			WinWindth = rect.right;
			WinHigh = rect.bottom;

			if (WinWindth < width || WinHigh < high) 
			{
				GetWindowRect(hwnd, &WinSize);

				if (WinWindth < width) {
					WinWindth = WinSize.right;
					SetWindowPos(hwnd, NULL, WinSize.left, WinSize.top, WinSize.left + width, WinSize.top, SWP_NOMOVE);
				}

				if (WinHigh < high) {
					WinHigh = WinSize.bottom;
					SetWindowPos(hwnd, NULL, WinSize.left, WinSize.top, WinSize.left, WinSize.top + high, SWP_NOMOVE);
				}


			}

			break;
		}

		case WM_MOUSEWHEEL:
		{
			if ((short)HIWORD(wParam) > 0) 
			{
				if (wParam & MK_SHIFT) 
				{
					spX += 1;
				}
				else 
				{
					spY -= 1;
				}
			}
			else 
			{
				if (wParam & MK_SHIFT) 
				{
					spX -= 1;
				}
				else 
				{
					spY += 1;
				}
			}
			break;
		}

		case WM_TIMER: 
		{
			time++;
			if (time == 20) {
				updateWin(0);
				time = 0;
			}
			else {
				updateWin(1);
			}

			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, TRUE);
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


void updateWin(int spFlag) 
{
	if (left <= 0 && spX <= 0 || left + width >= WinWindth && spX >= 0) 
	{
		spX = spX ? -spX : (left <= 0 ? 5 : -5);
	}

	if (top <= 0 && spY <= 0 || top + high >= WinHigh && spY >= 0) {
		spY = spY ? -spY : (top <= 0 ? 5 : -5);
	}

	if (spFlag == 1) 
	{
		if (spX != 0) 
		{
			spX += spX > 0 ? -1 : 1;
		}

		if (spY != 0) 
		{
			spY += spY > 0 ? -1 : 1;
		}
	}

	left += spX;
	top += spY;
}



