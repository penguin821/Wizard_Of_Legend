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
void animation(HDC hdc, CImage* img, int posx, int posy, int animCutX, int animCutY, TYPE type);
void cal_movement(DIR* dir, int* posx, int* posy, bool* input,bool* idle);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, memdc;
	static RECT c;
	static POINT mouse;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	static bool keyLayout[256];

	static CImage Logo,Target;
	static CImage StoneTile;
	static CImage PlayerFront, PlayerBack, PlayerLeft, PlayerRight;
	static CImage ArcherBowLeft, ArcherBowRight, ArcherLeft, ArcherRight;//몬스터1
	static CImage SwordmanLeft, SwordmanRight, SwordmanAttack;//몬스터3
	static Character pl, sw, ar, wz, bs; // 플레이어,소드맨,아처,위자드,보스
	static SCENE sceneNow;
	HBITMAP hBitmap;

	static bool isIdle;
	static int whereToGo = 4;
	static int howManyMove = 0;
	static bool left = TRUE;

	switch (uMsg)
	{
	case WM_CREATE: // 첫 초기화
	{
		// UI
		Logo.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\READY_MENU.bmp");
		Target.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\UI_MOUSE.bmp");
		GetClientRect(hWnd, &c);
		ShowCursor(false);
		sceneNow = SCENE_LOGO;

		// Map
		StoneTile.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\stonetile.bmp");

		// Player
		PlayerFront.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\FRONT_COMPLETE.bmp");
		PlayerBack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\BACK_COMPLETE.bmp");
		PlayerLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\LEFT_COMPLETE.bmp");
		PlayerRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\RIGHT_COMPLETE.bmp");
		pl.posX = 0, pl.posY = 0;
		pl.animPosX = 1, pl.animPosY = 2;
		pl.dir = DIR_DOWN;
		isIdle = true;

		// Monster
		ArcherBowLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_LEFT.bmp");
		ArcherBowRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_RIGHT.bmp");
		ArcherLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_LEFT.bmp");
		ArcherRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_RIGHT.bmp");

		SwordmanLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_LEFT.bmp");
		SwordmanRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_RIGHT.bmp");
		SwordmanAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_ATTACK.bmp");
		sw.posX = 950, sw.posY = 500;
		sw.animPosX = 1, sw.animPosY = 2;
		sw.ef_animPosX = 1;
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
			cal_movement(&pl.dir, &pl.posX, &pl.posY, keyLayout,&isIdle);
			if (!isIdle)
			{
				if (10 == pl.animPosX)
					pl.animPosX = 1;
				else
					pl.animPosX += 1;
			}

			if (2 == sw.animPosY) // 왼쪽 이동
			{
				if (6 == sw.animPosX)
					sw.animPosX = 1;
				else
					sw.animPosX += 1;
			}
			else if (3 == sw.animPosY) // 왼쪽 공격
			{
				if (3 == sw.animPosX)
					sw.animPosX = 1;
				else
					sw.animPosX += 1;
			}

			if (4 == sw.ef_animPosX)
				sw.ef_animPosX = 1;
			else
				sw.ef_animPosX += 1;


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

			if (pl.posX < sw.posX)
			{
				sw.posX -= 1;
				left = TRUE;
			}
			else if (pl.posX > sw.posX)
			{
				sw.posX += 1;
				left = FALSE;
			}
			if (pl.posX < sw.posY)
				sw.posY -= 1;
			else if (pl.posY > sw.posY)
				sw.posY += 1;

			InvalidateRect(hWnd, NULL, FALSE);
			break;

		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
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
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
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
				SetTimer(hWnd, 2, 20, NULL);
				//Logo.ReleaseDC();
			}
		}

		if ('a' == wParam || 'A' == wParam)
		{
			keyLayout[VK_LEFT] = 1;
			isIdle = false;
		}
		if ('d' == wParam || 'D' == wParam)
		{
			keyLayout[VK_RIGHT] = 1;
			isIdle = false;
		}
		if ('w' == wParam || 'W' == wParam)
		{
			keyLayout[VK_UP] = 1;
			isIdle = false;
		}
		if ('s' == wParam || 'S' == wParam)
		{
			keyLayout[VK_DOWN] = 1;
			isIdle = false;
		}
	}
	break;
	case WM_KEYUP:
	{
		if ('a' == wParam || 'A' == wParam)
		{
			keyLayout[VK_LEFT] = 0;
		}
		if ('d' == wParam || 'D' == wParam)
		{
			keyLayout[VK_RIGHT] = 0;
		}
		if ('w' == wParam || 'W' == wParam)
		{
			keyLayout[VK_UP] = 0;
		}
		if ('s' == wParam || 'S' == wParam)
		{
			keyLayout[VK_DOWN] = 0;
		}
	}
	break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		hBitmap = CreateCompatibleBitmap(hdc, c.right, c.bottom);
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap);
		
		//StretchBlt(mem1dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mem2dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SRCCOPY);

		if (SCENE_LOGO == sceneNow)
		{
			int w = Logo.GetWidth();
			int h = Logo.GetHeight();
			Logo.Draw(memdc, 0, 0, c.right, c.bottom, 0, 0, w, h);
		}
		else if (SCENE_STAGE == sceneNow)
		{
			create_stone_map(memdc, &StoneTile);

			//몬스터를 랜덤하게 이동할때 쓰이는 변수
			/*if (howManyMove % 10 == 0)
				whereToGo = direction(gen);*/
				//SelectObject(memdc, hBitmap);
			if (pl.posX + 180 < sw.posX + 200 && pl.posX + 180 > sw.posX)
			{
				if (pl.posY<sw.posY + 202 && pl.posY>sw.posY)
				{
					sw.animPosY = 3;
					sw.ef_animPosY = 1;
					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, sw.ef_animPosY, TYPE_SWORD_ATTACK);
					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, sw.animPosY, TYPE_SWORD_ATTACK);
				}
				else if (pl.posY + 180 < sw.posY + 202 && pl.posY + 180 > sw.posY)
				{
					sw.animPosY = 3;
					sw.ef_animPosY = 1;
					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, sw.ef_animPosY, TYPE_SWORD_ATTACK);
					animation(memdc, &SwordmanLeft, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
				else if (sw.posY<pl.posY + 182 && sw.posY>pl.posY)
				{
					sw.animPosY = 3;
					sw.ef_animPosY = 4;
					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, sw.ef_animPosY, TYPE_SWORD_ATTACK);
					animation(memdc, &SwordmanLeft, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
				else if (sw.posY + 202 < pl.posY + 182 && sw.posY + 202 > pl.posY)
				{

					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, 2, TYPE_SWORD_ATTACK);
				}
				else if (left == TRUE)
				{
					sw.animPosY = 2;
					animation(memdc, &SwordmanLeft, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
				else
				{
					sw.animPosY = 2;
					animation(memdc, &SwordmanRight, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}

			}
			else if (sw.posX + 200 < pl.posX + 180 && sw.posX + 200 >= pl.posX)
			{
				if (pl.posY<sw.posY + 202 && pl.posY>sw.posY)
				{
					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, 2, TYPE_SWORD_ATTACK);
					animation(memdc, &SwordmanRight, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
				else if (pl.posY + 180 < sw.posY + 202 && pl.posY + 180 > sw.posY)
				{
					sw.animPosY = 3;
					sw.ef_animPosY = 2;
					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, sw.ef_animPosY, TYPE_SWORD_ATTACK);
					animation(memdc, &SwordmanRight, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
				else if (sw.posY<pl.posY + 182 && sw.posY>pl.posY)
				{

					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, 2, TYPE_SWORD_ATTACK);
				}
				else if (sw.posY + 202 < pl.posY + 182 && sw.posY + 202 > pl.posY)
				{

					animation(memdc, &SwordmanAttack, sw.posX, sw.posY, sw.ef_animPosX, 2, TYPE_SWORD_ATTACK);
				}
				else if (left == TRUE)
				{
					sw.animPosY = 2;
					animation(memdc, &SwordmanLeft, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
				else
				{
					sw.animPosY = 2;
					animation(memdc, &SwordmanRight, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
				}
			}
			else if (left == TRUE)
			{
				sw.animPosY = 2;
				animation(memdc, &SwordmanLeft, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
			}
			else
			{
				sw.animPosY = 2;
				animation(memdc, &SwordmanRight, sw.posX, sw.posY, sw.animPosX, sw.animPosY, TYPE_SWORD);
			}


			//animation(mem2dc, &ArcherLeft, badXPos3, badYPos3, 1);

			if (DIR_DOWN == pl.dir)
				animation(memdc, &PlayerFront, pl.posX, pl.posY, pl.animPosX, pl.animPosY, TYPE_PLAYER);
			if (DIR_UP == pl.dir)
				animation(memdc, &PlayerBack, pl.posX, pl.posY, pl.animPosX, pl.animPosY, TYPE_PLAYER);
			if (DIR_LEFT == pl.dir)
				animation(memdc, &PlayerLeft, pl.posX, pl.posY, pl.animPosX, pl.animPosY, TYPE_PLAYER);
			if (DIR_RIGHT == pl.dir)
				animation(memdc, &PlayerRight, pl.posX, pl.posY, pl.animPosX, pl.animPosY, TYPE_PLAYER);

			Target.TransparentBlt(memdc, mouse.x - 30, mouse.y - 30, 60, 60, 0, 0, 60, 60, RGB(255, 0, 255)); // 마우스
		}
		BitBlt(hdc, 0, 0, c.right, c.bottom, memdc, 0, 0, SRCCOPY);

		DeleteObject(hBitmap);
		DeleteDC(memdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);

		Target.ReleaseDC();
		StoneTile.ReleaseDC();

		PlayerFront.ReleaseDC();
		PlayerBack.ReleaseDC();
		PlayerLeft.ReleaseDC();
		PlayerRight.ReleaseDC();

		ArcherBowLeft.ReleaseDC();
		ArcherBowRight.ReleaseDC();
		ArcherLeft.ReleaseDC();
		ArcherRight.ReleaseDC();

		SwordmanLeft.ReleaseDC();
		SwordmanRight.ReleaseDC();
		SwordmanAttack.ReleaseDC();

		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void create_stone_map(HDC hdc, CImage* img)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	for (int i = 0; i < 3; ++i)
		img->Draw(hdc, w * i, 0, w, h / 4 * 3, 0, 0, w, h / 4 * 3);
	img->Draw(hdc, w * 3, 0, w / 3 * 2, h / 4 * 3, 0, 0, w / 3 * 2, h / 4 * 3);

	for (int i = 0; i < 3; ++i)
		img->Draw(hdc, w * i, h / 4 * 3, w, h / 4, 0, h / 4, w, h / 4);
	img->Draw(hdc, w * 3, h / 4 * 3, w / 3 * 2, h / 4, 0, h / 4, w, h / 4);

	for (int i = 0; i < 3; ++i)
		img->Draw(hdc, w * i, h, w, h / 4 * 3, 0, 0, w, h / 4 * 3);
	img->Draw(hdc, w * 3, h, w / 3 * 2, h / 4 * 3, 0, 0, w / 3 * 2, h / 4 * 3);


}

void animation(HDC hdc, CImage* img, int posx, int posy, int animCutX, int animCutY, TYPE type)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	if (TYPE_PLAYER == type)
	{
		img->TransparentBlt(hdc, posx, posy, 180, 182, 180 * (animCutX - 1), 182 * (animCutY - 1), 180, 182, RGB(255, 0, 255));
	}
	else if (TYPE_SWORD == type)
	{
		img->TransparentBlt(hdc, posx, posy, 200, 202, 200 * (animCutX - 1), 202 * (animCutY - 1), 200, 202, RGB(255, 0, 255));
	}
	else if (TYPE_SWORD_ATTACK == type)
	{
		img->TransparentBlt(hdc, posx, posy, 200, 200, 200 * (animCutX - 1), 200 * (animCutY - 1), 200, 200, RGB(255, 0, 255));
	}
}

void cal_movement(DIR* dir, int* posx, int* posy, bool* input, bool* idle)
{
	POINT move;

	if (input[VK_LEFT] == input[VK_RIGHT])
		move.x = 0.f;
	else if (input[VK_LEFT])
	{
		*idle = false;
		*dir = DIR_LEFT;
		move.x = -10.f;
	}
	else
	{
		*idle = false;
		*dir = DIR_RIGHT;
		move.x = 10.f;
	}


	if (input[VK_UP] == input[VK_DOWN])
		move.y = 0.f;
	else if (input[VK_UP])
	{
		*idle = false;
		*dir = DIR_UP;
		move.y = -10.f;
	}
	else
	{
		*idle = false;
		*dir = DIR_DOWN;
		move.y = 10.f;
	}

	if ((input[VK_UP] == true && input[VK_LEFT] == true) || input[VK_UP] == true && input[VK_RIGHT] == true)
	{
		*dir = DIR_UP;
	}
	else if ((input[VK_DOWN] == true && input[VK_LEFT] == true) || input[VK_DOWN] == true && input[VK_RIGHT] == true)
	{
		*dir = DIR_DOWN;
	}

	if (input[VK_UP] == input[VK_DOWN] == input[VK_LEFT] == input[VK_RIGHT])
		*idle = true;

	*posx += move.x;
	*posy += move.y;
}

