#include "pch.h"

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
	//WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	Window = CreateWindow(lpszClass, L"Wizard of Legend", WS_OVERLAPPEDWINDOW,
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

void animationBad3(HDC hdc, CImage* img, int posx, int posy, int animCut);
void animationBad3attack(HDC hdc, CImage* img, int posx, int posy, int animCut);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT c;
	static POINT mouse;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;

	HDC mem1dc;
	static CImage Logo, Img, ImgSprite;
	static int xPos, yPos;
	static int animxPos, animyPos;
	static int w, h;
	static int sceneNow;
	HBITMAP hBitmap;
	static int badXPos3, badYPos3;
	static int bad3AnimxPos, bad3AnimyPos, bad3attack;
	static bool left = TRUE;

	switch (uMsg)
	{
	case WM_CREATE: // 첫 초기화
	{
		Logo.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\READY_MENU.bmp");
		//Img.Load(L"bmp_chu.bmp");
		ImgSprite.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\FRONT_COMPLETE.bmp");

		xPos = 0, yPos = 0;
		animxPos = 1;
		sceneNow = 0;

		SetTimer(hWnd, 1, 100, NULL);
		break;
	}
	break;
	case WM_COMMAND:// 메뉴
	{
		//switch (LOWORD(wParam))
		//{
		//case ID_GAME_START:
		//	break;
		//}
	}
	break;
	case WM_CHAR:
	{
		if (wParam == 'q' || wParam == 'Q')
		{
			PostQuitMessage(0);
		}
	}
	break;
	case WM_TIMER:
	{
		switch (wParam)
		{

		case 1:
		{
			if (10 == animxPos)
				animxPos = 1;
			else
				animxPos += 1;
			//yPos += 5; 
			InvalidateRect(hWnd, NULL, FALSE);
		}

		case 2:
			//몬스터를 랜덤하게 이동
			//switch (whereToGo)
			//{
			//case 0://up
			//	if (c.top > badYPos3)
			//		whereToGo = 1;
			//	else
			//	{
			//		badYPos3 -= 10;
			//		++howManyMove;
			//	}
			//	break;
			//case 1://down
			//	if (c.bottom < badYPos3+100)
			//		whereToGo = 0;
			//	else
			//	{
			//		badYPos3 += 10;
			//		++howManyMove;
			//	}
			//	break;
			//case 2://right
			//	if (c.right < badYPos3+50)
			//		whereToGo = 3;
			//	else
			//	{
			//		badXPos3 += 10;
			//		++howManyMove;
			//	}
			//	break;
			//case 3://left
			//	if (c.left > badYPos3)
			//		whereToGo = 2;
			//	else
			//	{
			//		badXPos3 -= 10;
			//		++howManyMove;
			//	}
			//	break;
			//}

			if (xPos < badXPos3)
			{
				badXPos3 -= 1;
				left = TRUE;
			}
			else if (xPos > badXPos3)
			{
				badXPos3 += 1;
				left = FALSE;
			}
			if (yPos < badYPos3)
				badYPos3 -= 1;
			else if (yPos > badYPos3)
				badYPos3 += 1;

			InvalidateRect(hWnd, NULL, FALSE);
			break;

		}
	}
	break;
	case WM_LBUTTONDOWN:
	{

	}
	InvalidateRect(hWnd, NULL, FALSE);
	break;
	case WM_LBUTTONUP:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		//if (PtInRect(&light[i].self, mouse)) // 사각형-마우스 충돌체크
		//{
		//	selected = i;
		//	break;
		//}
	}
	break;
	case WM_MOUSEMOVE:
	{

	}
	break;
	case WM_KEYDOWN:
	{
		if (VK_LEFT == wParam)
		{
			xPos -= 5;
		}
		if (VK_RIGHT == wParam)
		{
			xPos += 5;
		}
		if (VK_UP == wParam)
		{
			yPos -= 5;
		}
		if (VK_DOWN == wParam)
		{
			yPos += 5;
		}
	}
	case WM_PAINT:
	{
		GetClientRect(hWnd, &c);
		hdc = BeginPaint(hWnd, &ps);
		hBitmap = CreateCompatibleBitmap(hdc, c.right, c.bottom);
		mem1dc = CreateCompatibleDC(hdc);
		SelectObject(mem1dc, hBitmap);

		if (SCENE_LOGO == sceneNow)
		{
			w = Logo.GetWidth();
			h = Logo.GetHeight();
			Logo.Draw(mem1dc, 0, 0, c.right, c.bottom, 0, 0, w, h);
			BitBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, SRCCOPY);
			//TransparentBlt(hdc, 0, 0, 180, 182, mem1dc, 180 * (animxPos - 1), 182 * 1, 180, 182, RGB(255, 0, 255));
		}
		else if (SCENE_STAGE == sceneNow)
		{
			w = Img.GetWidth();
			h = Img.GetHeight();
			//Img.Draw(mem1dc, 0, 0, c.right, c.bottom, 0, 0, w, h);
			ImgSprite.Draw(mem1dc, xPos, yPos, 180, 182, 180 * (animxPos - 1), 182 * 1, 180, 182);
			BitBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, SRCCOPY);
			TransparentBlt(hdc, 0, 0, 180, 182, mem1dc, 180 * (animxPos - 1), 182 * 1, 180, 182, RGB(255, 0, 255));
		}

		DeleteObject(hBitmap);
		DeleteDC(mem1dc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
void animationBad3(HDC hdc, CImage* img, int posx, int posy, int animCut)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	img->Draw(hdc, posx, posy, 200, 202, 200 * (animCut - 1), 202 * 1, 200, 202);
}
void animationBad3attack(HDC hdc, CImage* img, int posx, int posy, int animCut)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	img->Draw(hdc, posx, posy, 200, 200, 200 * (animCut - 1), 200 * 1, 200, 200);
}