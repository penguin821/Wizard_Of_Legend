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

void create_stone_map(HDC hdc, CImage* img);
void animation(HDC hdc, CImage* img, int posx, int posy, int animCut);
void cal_movement(DIR* dir, int* posx, int* posy, bool* input);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT c;
	static POINT mouse;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	static bool keyLayout[256];

	HDC mem1dc,mem2dc;
	static CImage Logo;
	static CImage StoneTile;
	static CImage PlayerFront, PlayerBack, PlayerLeft, PlayerRight;
	static CImage ArcherBowLeft, ArcherBowRight, ArcherLeft, ArcherRight;//몬스터1
	static CImage SwordmanLeft, SwordmanRight, SwordmanAttack;//몬스터3
	static int xPos, yPos;
	static int animxPos, animyPos;
	static SCENE sceneNow;
	HBITMAP hBitmap;

	static bool isIdle;
	static DIR dir_pl, dir_boss, dir_mon;

	switch (uMsg)
	{
	case WM_CREATE: // 첫 초기화
	{
		Logo.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\READY_MENU.bmp");
		StoneTile.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\stonetile.bmp");
		PlayerFront.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\FRONT_COMPLETE.bmp");
		PlayerBack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\BACK_COMPLETE.bmp");
		PlayerLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\LEFT_COMPLETE.bmp");
		PlayerRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\RIGHT_COMPLETE.bmp");

		ArcherBowLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_LEFT.bmp");
		ArcherBowRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_RIGHT.bmp");
		ArcherLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_LEFT.bmp");
		ArcherRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_RIGHT.bmp");

		SwordmanLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\SWORDMAN_LEFT.bmp");
		SwordmanRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\SWORDMAN_RIGHT.bmp");
		SwordmanAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\SWORDMAN_ATTACK.bmp");

		xPos = 0, yPos = 0;
		animxPos = 1;
		sceneNow = SCENE_LOGO;
		dir_pl = DIR_DOWN;
		isIdle = true;
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
			cal_movement(&dir_pl, &xPos, &yPos, keyLayout);
			if (!isIdle)
			{
				if (10 == animxPos)
					animxPos = 1;
				else
					animxPos += 1;
			}
			//yPos += 5; 
			InvalidateRect(hWnd, NULL, FALSE);
		}

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
		if (SCENE_LOGO == sceneNow)
		{
			if (wParam)
			{
				sceneNow = SCENE_STAGE;
				SetTimer(hWnd, 1, 50, NULL);
			}
		}
		if (VK_LEFT == wParam)
		{
			keyLayout[VK_LEFT] = 1;
			isIdle = false;
		}
		if (VK_RIGHT == wParam)
		{
			keyLayout[VK_RIGHT] = 1;
			isIdle = false;
		}
		if (VK_UP == wParam)
		{
			keyLayout[VK_UP] = 1;
			isIdle = false;
		}
		if (VK_DOWN == wParam)
		{
			keyLayout[VK_DOWN] = 1;
			isIdle = false;
		}
	}
	break;
	case WM_KEYUP:
	{
		if (VK_LEFT == wParam)
		{
			keyLayout[VK_LEFT] = 0;
		}
		if (VK_RIGHT == wParam)
		{
			keyLayout[VK_RIGHT] = 0;
		}
		if (VK_UP == wParam)
		{
			keyLayout[VK_UP] = 0;
		}
		if (VK_DOWN == wParam)
		{
			keyLayout[VK_DOWN] = 0;
		}
	}
	break;
	case WM_PAINT:
	{
		GetClientRect(hWnd, &c);
		hdc = BeginPaint(hWnd, &ps);
		hBitmap = CreateCompatibleBitmap(hdc, c.right, c.bottom);
		mem1dc = CreateCompatibleDC(hdc);
		mem2dc = CreateCompatibleDC(mem1dc);
		SelectObject(mem1dc, hBitmap);

		if (SCENE_LOGO == sceneNow)
		{
			int w = Logo.GetWidth();
			int h = Logo.GetHeight();
			Logo.Draw(mem1dc, 0, 0, c.right, c.bottom, 0, 0, w, h);
			BitBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, SRCCOPY);
		}
		else if (SCENE_STAGE == sceneNow)
		{
			create_stone_map(mem1dc, &StoneTile);
			create_stone_map(mem2dc, &StoneTile);

			if (DIR_DOWN == dir_pl)
				animation(mem1dc, &PlayerFront, xPos, yPos, animxPos);
			if (DIR_UP == dir_pl)
				animation(mem1dc, &PlayerBack, xPos, yPos, animxPos);
			if (DIR_LEFT == dir_pl)
				animation(mem1dc, &PlayerLeft, xPos, yPos, animxPos);
			if (DIR_RIGHT == dir_pl)
				animation(mem1dc, &PlayerRight, xPos, yPos, animxPos);

			//BitBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, SRCCOPY); // 메모리dc에 저장된 이미지를 고속으로 화면에 띄우는 역할
			TransparentBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, c.right, c.bottom, RGB(255, 0, 255));
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

void create_stone_map(HDC hdc, CImage* img)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	img->Draw(hdc, 0, 0, w / 3, h / 4, 0, 0, w / 3, h / 4);
	img->Draw(hdc, w / 3, 0, w / 3, h / 4, w / 3, h / 4, w / 3, h / 4);
}

void animation(HDC hdc, CImage* img, int posx, int posy, int animCut)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	img->Draw(hdc, posx, posy, 180, 182, 180 * (animCut - 1), 182 * 1, 180, 182);
}

void cal_movement(DIR* dir, int* posx, int* posy, bool* input)
{
	POINT offset;

	if (input[VK_LEFT] == input[VK_RIGHT])
		offset.x = 0.f;
	else if (input[VK_LEFT])
	{
		*dir = DIR_LEFT;
		offset.x = -10.f;
	}
	else
	{
		*dir = DIR_RIGHT;
		offset.x = 10.f;
	}


	if (input[VK_UP] == input[VK_DOWN])
		offset.y = 0.f;
	else if (input[VK_UP])
	{
		*dir = DIR_UP;
		offset.y = -10.f;
	}
	else
	{
		*dir = DIR_DOWN;
		offset.y = 10.f;
	}

	*posx += offset.x;
	*posy += offset.y;
}
