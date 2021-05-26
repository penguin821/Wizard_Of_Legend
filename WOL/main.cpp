#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <ctime>

#pragma comment (lib, "msimg32.lib")

#define _MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);

constexpr int WINDOW_WIDTH = 900;
constexpr int WINDOW_HEIGHT = 900;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	srand(time(NULL));
	HWND Window;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	Window = CreateWindow(lpszClass, L"Wizard Of Legend", WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(Window, nCmdShow);
	UpdateWindow(Window);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, mem1dc, mem2dc;
	PAINTSTRUCT ps;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;

	static POINT mouse;
	static RECT c;
	static HBITMAP hBit1, hBit2, oldBit1, oldBit2;
	//bool compile = FALSE;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		GetClientRect(hWnd, &c);
		break;
	}
	break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case 1:
		{
			break;
		}
		}
	}
	case WM_KEYDOWN:
	{
		if (VK_LEFT == wParam)
		{

		}
		if (VK_RIGHT == wParam)
		{

		}
		if (VK_UP == wParam)
		{

		}
		if (VK_DOWN == wParam)
		{

		}
	}
	InvalidateRect(hWnd, NULL, FALSE);
	break;
	case WM_LBUTTONUP:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
	}
	break;
	case WM_PAINT:
	{
		GetClientRect(hWnd, &c);
		hdc = BeginPaint(hWnd, &ps);
		mem1dc = CreateCompatibleDC(hdc);

		//--- hBit1에는 배경과 텍스트가 출력된 비트맵이 저장되어 있다. 이 비트맵을 mem1dc에 선택 
		oldBit1 = (HBITMAP)SelectObject(mem1dc, hBit1);

		//--- mem1dc에 있는 내용을 hdc에 복사한다. 
		BitBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, SRCCOPY);

		SelectObject(mem1dc, oldBit1);
		DeleteDC(mem1dc);
		EndPaint(hWnd, &ps);
		break;
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}