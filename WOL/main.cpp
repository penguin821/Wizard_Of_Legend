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
void animationBad3(HDC hdc, CImage* img, int posx, int posy, int animCut);
void animationBad3attack(HDC hdc, CImage* img, int posx, int posy, int animCut);
void cal_movement(DIR* dir, int* posx, int* posy, bool* input);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static RECT c;
	static POINT mouse;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	static bool keyLayout[256];

	HDC mem1dc, mem2dc;
	static CImage Logo;
	static CImage StoneTile;
	static CImage PlayerFront, PlayerBack, PlayerLeft, PlayerRight;
	static CImage ArcherBowLeft, ArcherBowRight, ArcherLeft, ArcherRight;//몬스터1
	static CImage SwordmanLeft, SwordmanRight, SwordmanAttack;//몬스터3
	static int xPos, yPos;
	static int badXPos3, badYPos3;
	static int animxPos, animyPos;
	static int bad3AnimxPos, bad3AnimyPos, bad3attack;
	static SCENE sceneNow;
	HBITMAP hBitmap, hBit, oldBit, oldBit2;

	static bool isIdle;
	static DIR dir_pl, dir_boss, dir_mon;
	static int whereToGo = 4;
	static int howManyMove = 0;
	static bool left = TRUE;

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
		badXPos3 = 950, badYPos3 = 500;
		animxPos = 1;
		bad3AnimxPos = 1;
		sceneNow = SCENE_LOGO;
		dir_pl = DIR_DOWN;
		isIdle = true;
		GetClientRect(hWnd, &c);

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

			if (6 == bad3AnimxPos)
				bad3AnimxPos = 1;
			else
				bad3AnimxPos += 1;

			if (4 == bad3attack)
				bad3attack = 1;
			else
				bad3attack += 1;


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
		if (SCENE_LOGO == sceneNow)
		{
			if (wParam)
			{
				sceneNow = SCENE_STAGE;
				SetTimer(hWnd, 1, 50, NULL);
				SetTimer(hWnd, 2, 20, NULL);

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
		//GetClientRect(hWnd, &c);
		hdc = BeginPaint(hWnd, &ps);

		hBitmap = CreateCompatibleBitmap(hdc, c.right, c.bottom);
		mem1dc = CreateCompatibleDC(hdc);
		mem2dc = CreateCompatibleDC(mem1dc);

		hBit = CreateCompatibleBitmap(hdc, c.right, c.bottom);
		oldBit = (HBITMAP)SelectObject(mem1dc, hBit);
		oldBit2 = (HBITMAP)SelectObject(mem2dc, hBitmap);


		StretchBlt(mem1dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mem2dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SRCCOPY);

		if (SCENE_LOGO == sceneNow)
		{
			int w = Logo.GetWidth();
			int h = Logo.GetHeight();
			Logo.Draw(mem2dc, 0, 0, c.right, c.bottom, 0, 0, w, h);
			BitBlt(mem1dc, 0, 0, c.right, c.bottom, mem2dc, 0, 0, SRCCOPY);
		}
		else if (SCENE_STAGE == sceneNow)
		{
			create_stone_map(mem2dc, &StoneTile);// 캐릭터 주위로 검은박스
			create_stone_map(mem1dc, &StoneTile);// 캐릭터 주위에만 타일

			//몬스터를 랜덤하게 이동할때 쓰이는 변수
			/*if (howManyMove % 10 == 0)
				whereToGo = direction(gen);*/

			if (xPos + 180 < badXPos3 + 200 && xPos + 180 > badXPos3)
			{
				if (yPos<badYPos3 + 202 && yPos>badYPos3)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (yPos + 180 < badYPos3 + 202 && yPos + 180 > badYPos3)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (badYPos3<yPos + 182 && badYPos3>yPos)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (badYPos3 + 202 < yPos + 182 && badYPos3 + 202 > yPos)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (left == TRUE)
					animationBad3(mem2dc, &SwordmanLeft, badXPos3, badYPos3, bad3AnimxPos);
				else
					animationBad3(mem2dc, &SwordmanRight, badXPos3, badYPos3, bad3AnimxPos);

			}
			else if (badXPos3 + 200 < xPos + 180 && badXPos3 + 200 >= xPos)
			{
				if (yPos<badYPos3 + 202 && yPos>badYPos3)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (yPos + 180 < badYPos3 + 202 && yPos + 180 > badYPos3)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (badYPos3<yPos + 182 && badYPos3>yPos)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (badYPos3 + 202 < yPos + 182 && badYPos3 + 202 > yPos)
					animationBad3attack(mem2dc, &SwordmanAttack, badXPos3, badYPos3, bad3attack);
				else if (left == TRUE)
					animationBad3(mem2dc, &SwordmanLeft, badXPos3, badYPos3, bad3AnimxPos);
				else
					animationBad3(mem2dc, &SwordmanRight, badXPos3, badYPos3, bad3AnimxPos);
			}
			else if (left == TRUE)
				animationBad3(mem2dc, &SwordmanLeft, badXPos3, badYPos3, bad3AnimxPos);
			else
				animationBad3(mem2dc, &SwordmanRight, badXPos3, badYPos3, bad3AnimxPos);


			//animation(mem2dc, &ArcherLeft, badXPos3, badYPos3, 1);

			if (DIR_DOWN == dir_pl)
				animation(mem2dc, &PlayerFront, xPos, yPos, animxPos);
			if (DIR_UP == dir_pl)
				animation(mem2dc, &PlayerBack, xPos, yPos, animxPos);
			if (DIR_LEFT == dir_pl)
				animation(mem2dc, &PlayerLeft, xPos, yPos, animxPos);
			if (DIR_RIGHT == dir_pl)
				animation(mem2dc, &PlayerRight, xPos, yPos, animxPos);


			//BitBlt(hdc, 0, 0, c.right, c.bottom, mem1dc, 0, 0, SRCCOPY); // 메모리dc에 저장된 이미지를 고속으로 화면에 띄우는 역할
			TransparentBlt(mem1dc, 0, 0, c.right, c.bottom, mem2dc, 0, 0, c.right, c.bottom, RGB(255, 0, 255));
		}


		StretchBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mem1dc, 0, 0, c.right, c.bottom, SRCCOPY);

		DeleteObject(hBitmap);
		DeleteDC(mem1dc);
		DeleteObject(hBit);
		DeleteDC(mem2dc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		
		Logo.ReleaseDC();
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

void animation(HDC hdc, CImage* img, int posx, int posy, int animCut)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	img->Draw(hdc, posx, posy, 180, 182, 180 * (animCut - 1), 182 * 1, 180, 182);
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

