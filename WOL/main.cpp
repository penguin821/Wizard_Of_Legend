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
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	Window = CreateWindow(lpszClass, L"Wizard of Legend", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
		WS_BORDER | WS_MINIMIZEBOX, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(Window, nCmdShow);
	UpdateWindow(Window);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}

void create_stone_map(HDC hdc, CImage* img, CImage* img2, CImage* img3, CImage* img4, CImage* img5, CImage* img6, CImage* img7, CImage* img8, CImage* img9, CImage* img10
	,CImage* img11);
void animation(HDC hdc, CImage* img, Character* ch, TYPE type);
void animation(HDC hdc, CImage* img, const Effect& ch, ELEMENT type);
void cal_movement(DIR* dir, int* posx, int* posy, bool* input, bool* idle);
DIR check_collision(Character* a, Character* b);
DIR check_collision(Character* a, Effect* b);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, memdc;
	static RECT c;
	static POINT mouse, camera;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	static bool keyLayout[256];

	static CImage Logo, Target, Summon;
	static CImage StoneTile, MAPTILE_1_2, MAPTILE_2_1, MAPTILE_2_2, MAPTILE_3_1, MAPTILE_3_2, MAPTILE_4_1, borderWidth, statue, horizontalWell,chairLeft;
	static CImage PlayerFront, PlayerBack, PlayerLeft, PlayerRight;
	static CImage ArcherBowLeft, ArcherBowRight, ArcherLeft, ArcherRight; // ����1
	static CImage SwordmanLeft, SwordmanRight, SwordmanAttack; // ����3
	static CImage Flame, IceCard, IceAttack, IceParticle, Wind; // ����Ʈ
	static Character pl, sw, ar, wz, bs; // �÷��̾�,�ҵ��,��ó,���ڵ�,����
	static SCENE sceneNow;
	HBITMAP hBitmap;
	static vector<Effect> ice, ice_end;

	static bool isIdle;
	static int whereToGo = 4;
	static int howManyMove = 0;
	static int winposX, winposY, centerX, centerY;
	switch (uMsg)
	{
	case WM_CREATE: // ù �ʱ�ȭ
	{
		// UI
		Logo.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\READY_MENU.bmp");
		Target.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\UI_MOUSE.bmp");
		Summon.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\UI_MOUSE.bmp");
		GetClientRect(hWnd, &c);
		ShowCursor(false);
		sceneNow = SCENE_LOGO;

		// Map
		StoneTile.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\stonetile.bmp");
		MAPTILE_1_2.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\MAPTILE_1_2.bmp");
		MAPTILE_2_1.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\MAPTILE_2_1.bmp");
		MAPTILE_2_2.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\MAPTILE_2_2.bmp");
		MAPTILE_3_1.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\MAPTILE_3_1.bmp");
		MAPTILE_3_2.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\MAPTILE_3_2.bmp");
		MAPTILE_4_1.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\MAPTILE_4_1.bmp");
		borderWidth.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\borderWidth.bmp");
		statue.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\STATUE.bmp");
		horizontalWell.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\horizontalWell.bmp");
		chairLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\CHAIR_LEFT.bmp");

		// Player
		PlayerFront.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\FRONT_COMPLETE.bmp");
		PlayerBack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\BACK_COMPLETE.bmp");
		PlayerLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\LEFT_COMPLETE.bmp");
		PlayerRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\RIGHT_COMPLETE.bmp");
		pl.posX = 1500, pl.posY = 1500, pl.animPosX = 1, pl.animPosY = 2;
		pl.sizeX = 180, pl.sizeY = 182, pl.hp = 100;
		pl.dir = DIR_DOWN, pl.type = TYPE_PLAYER, pl.st = ST_IDLE;
		isIdle = true;

		// Monster
		ArcherBowLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_LEFT.bmp");
		ArcherBowRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_RIGHT.bmp");
		ArcherLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_LEFT.bmp");
		ArcherRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_RIGHT.bmp");

		SwordmanLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_LEFT.bmp");
		SwordmanRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_RIGHT.bmp");
		SwordmanAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_ATTACK.bmp");
		sw.posX = 950, sw.posY = 500, sw.animPosX = 1, sw.animPosY = 2;
		sw.sizeX = 200, sw.sizeY = 202, sw.hp = 100;
		sw.ef_sizeX = 200, sw.ef_sizeY = 200, sw.ef_animPosX = 1;
		sw.type = TYPE_SWORD, sw.st = ST_IDLE;

		// Effect
		IceAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\ICE_CRYSTAL.bmp");
		IceParticle.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\ICE_BLAST.bmp");
		//IceCard.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\ICE_KRYSTAL_CARD.bmp");
	}
	break;
	case WM_COMMAND:// �޴�
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
			if (!keyLayout[VK_LEFT] && !keyLayout[VK_UP] && !keyLayout[VK_DOWN] && !keyLayout[VK_RIGHT])
				pl.animPosY = 1;
			cal_movement(&pl.dir, &pl.posX, &pl.posY, keyLayout, &isIdle);

			centerX = pl.posX + pl.sizeX / 2;
			centerY = pl.posY + pl.sizeY / 2;

			winposX = centerX - WINDOW_WIDTH / 2;
			winposY = centerY - WINDOW_HEIGHT / 2;
			if (winposX < 0)
				winposX = 0;
			if (winposY < 0)
				winposY = 0;
			if (winposX > WORLD_WIDTH)
				winposX = 0;
			if (winposX > WORLD_HEIGHT)
				winposY = 0;

			if (1 == pl.animPosY)
				pl.animPosX = 1;
			if (2 == pl.animPosY)
			{
				if (10 <= pl.animPosX)
					pl.animPosX = 1;
				else
					pl.animPosX += 1;
			}
			if (6 == pl.animPosY)
			{
				if (2 <= pl.animPosX)
					pl.animPosX = 1;
				else
					pl.animPosX += 1;
			}

			if (2 == sw.animPosY) // ���� �̵�
			{
				if (6 <= sw.animPosX)
					sw.animPosX = 1;
				else
					sw.animPosX += 1;
			}
			else if (3 == sw.animPosY) // ���� ����
			{
				if (3 <= sw.animPosX)
					sw.animPosX = 1;
				else
					sw.animPosX += 1;
			}
			else if (4 == sw.animPosY) // ���� �̵�
			{
				if (2 <= sw.animPosX)
					sw.animPosX = 1;
				else
					sw.animPosX += 1;
			}

			if (4 <= sw.ef_animPosX)
				sw.ef_animPosX = 1;
			else
				sw.ef_animPosX += 1;

			if (0 != ice.size())
			{
				for (auto it = ice.begin(); it != ice.end();)
				{
					if (it->posX == it->endPosX && it->posY == it->endPosY)
					{
						//Effect temp={ it->endPosX ,it->endPosY ,it->endPosX ,it->endPosY }
						//ice_end.emplace_back(it);
						//animation(memdc, &IceParticle, a, EL_ICE_END);
						it = ice.erase(it);
					}
					else
					{
						it->mid += 0.1f;
						it->posX = it->posX * (1 - it->mid) + it->mid * it->endPosX;
						it->posY = it->posY * (1 - it->mid) + it->mid * it->endPosY;

						if (18 <= it->animPosX)
							it->animPosX = 1;
						else
							it->animPosX += 1;
						++it;
					}
				}

				for (int i = 0; i < ice.size(); ++i)
					check_collision(&sw, &ice[i]);
				InvalidateRect(hWnd, NULL, FALSE);
			}

			/*if (0 != ice_end.size())
			{
			}*/
		}
		case 2:
			//���͸� �����ϰ� �̵�
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
			sw.animPosY = 2;
			if (pl.posX < sw.posX)
			{
				sw.dir = DIR_LEFT;
				sw.posX -= 2;
			}
			else if (pl.posX > sw.posX)
			{
				sw.dir = DIR_RIGHT;
				sw.posX += 2;
			}

			if (pl.posY < sw.posY)
				sw.posY -= 2;
			else if (pl.posY > sw.posY)
				sw.posY += 2;
			check_collision(&pl, &sw);

			InvalidateRect(hWnd, NULL, FALSE);
			break;

		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		int realMouseX = winposX + mouse.x;
		int realMouseY = winposY + mouse.y;

		if (EL_ICE == pl.el)
		{
			int w = IceAttack.GetWidth();
			int h = IceAttack.GetHeight();
			Effect temp = { pl.posX,pl.posY,realMouseX ,realMouseY,w / 18,h, 1, 1, 0, EL_ICE };
			ice.emplace_back(temp);
		}

		//X_vector = pos.X - player.left; ���콺 �߻� ����
		//Y_vector = pos.Y - player.top;

		//Distance = sqrtf(powf(X_vector, 2)) + sqrtf(powf(Y_vector, 2));

		//X_vector = X_vector / Distance;
		//Y_vector = Y_vector / Distance;

		//Missile_speed_X = X_vector * 1.0f;
		//Missile_speed_Y = Y_vector * 1.0f;
	}
	InvalidateRect(hWnd, NULL, FALSE);
	break;
	case WM_LBUTTONUP:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
	}
	break;
	case WM_MOUSEMOVE:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
		GetClientRect(hWnd, &c);
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
				SetTimer(hWnd, 2, 50, NULL);
				//Logo.ReleaseDC();
			}
		}

		if ('a' == wParam || 'A' == wParam)
		{
			keyLayout[VK_LEFT] = 1;
			pl.animPosY = 2;
			check_collision(&pl, &sw);
		}
		if ('d' == wParam || 'D' == wParam)
		{
			keyLayout[VK_RIGHT] = 1;
			pl.animPosY = 2;
			check_collision(&pl, &sw);
		}
		if ('w' == wParam || 'W' == wParam)
		{
			keyLayout[VK_UP] = 1;
			pl.animPosY = 2;
			check_collision(&pl, &sw);
		}
		if ('s' == wParam || 'S' == wParam)
		{
			keyLayout[VK_DOWN] = 1;
			pl.animPosY = 2;
			check_collision(&pl, &sw);
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

		hBitmap = CreateCompatibleBitmap(hdc, WORLD_WIDTH, WORLD_HEIGHT);
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap);

		//StretchBlt(mem1dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mem2dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SRCCOPY);

		if (SCENE_LOGO == sceneNow)
		{
			int w = Logo.GetWidth();
			int h = Logo.GetHeight();
			Logo.Draw(memdc, 0, 0, c.right, c.bottom, 0, 0, w, h);
			BitBlt(hdc, c.left, c.top, c.right, c.bottom, memdc, 0, 0, SRCCOPY);
		}
		else if (SCENE_STAGE == sceneNow)
		{
			create_stone_map(memdc, &StoneTile, &MAPTILE_1_2, &MAPTILE_2_1, &MAPTILE_2_2, &MAPTILE_3_1, &MAPTILE_3_2, &MAPTILE_4_1, &borderWidth, &statue, &horizontalWell,
				&chairLeft);

			//���͸� �����ϰ� �̵��Ҷ� ���̴� ����
			/*if (howManyMove % 10 == 0)
				whereToGo = direction(gen);*/

			if (DIR_LEFT == sw.dir)
			{
				if (3 == sw.animPosY) // Į �ֵθ��� ��� ���̸� �ܻ�����Ʈ�� �׷�����
				{
					sw.ef_animPosY = 1;
					animation(memdc, &SwordmanAttack, &sw, TYPE_EFFECT);
				}
				animation(memdc, &SwordmanLeft, &sw, TYPE_SWORD);
			}
			if (DIR_RIGHT == sw.dir)
			{
				if (3 == sw.animPosY) // Į �ֵθ��� ��� ���̸� �ܻ�����Ʈ�� �׷�����
				{
					sw.ef_animPosY = 2;
					animation(memdc, &SwordmanAttack, &sw, TYPE_EFFECT);
				}
				animation(memdc, &SwordmanRight, &sw, TYPE_SWORD);
			}

			if (DIR_DOWN == pl.dir)
			{
				animation(memdc, &PlayerFront, &pl, TYPE_PLAYER);
			}
			if (DIR_UP == pl.dir)
			{
				animation(memdc, &PlayerBack, &pl, TYPE_PLAYER);
			}
			if (DIR_LEFT == pl.dir)
			{
				animation(memdc, &PlayerLeft, &pl, TYPE_PLAYER);
			}
			if (DIR_RIGHT == pl.dir)
			{
				animation(memdc, &PlayerRight, &pl, TYPE_PLAYER);
			}
			if (0 != ice.size())
			{
				for (const auto& a : ice)
				{
					animation(memdc, &IceAttack, a, EL_ICE);
				}
			}

			Target.TransparentBlt(memdc, winposX + mouse.x - 30, winposY + mouse.y - 30, 60, 60, 0, 0, 60, 60, RGB(255, 0, 255)); // ���콺
			BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
				memdc, winposX, winposY, SRCCOPY);
		}

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

void create_stone_map(HDC hdc, CImage* img, CImage* img2, CImage* img3, CImage* img4, CImage* img5, CImage* img6, CImage* img7, CImage* img8, CImage* img9, CImage* img10
	, CImage* img11)
{
	/*for (int i = 0; i < PATTERN_SIZE; ++i)
		img->Draw(hdc, w * i, 0, w, h / 4 * 3, 0, 0, w, h / 4 * 3);
	img->Draw(hdc, w * 3, 0, w / 3 * 2, h / 4 * 3, 0, 0, w / 3 * 2, h / 4 * 3);

	for (int i = 0; i < PATTERN_SIZE; ++i)
		img->Draw(hdc, w * i, h / 4 * 3, w, h / 4, 0, h / 4, w, h / 4);
	img->Draw(hdc, w * 3, h / 4 * 3, w / 3 * 2, h / 4, 0, h / 4, w, h / 4);

	for (int i = 0; i < PATTERN_SIZE; ++i)
		img->Draw(hdc, w * i, h, w, h / 4 * 3, 0, 0, w, h / 4 * 3);
	img->Draw(hdc, w * 3, h, w / 3 * 2, h / 4 * 3, 0, 0, w / 3 * 2, h / 4 * 3);*/
	int PATTERN_SIZE = 8;
	int w = img->GetWidth();
	int h = img->GetHeight();

	int w2 = img2->GetWidth();
	int h2 = img2->GetHeight();

	int w3 = img3->GetWidth();
	int h3 = img3->GetHeight();

	int w4 = img4->GetWidth();
	int h4 = img4->GetHeight();

	int w5 = img5->GetWidth();
	int h5 = img5->GetHeight();

	int w6 = img6->GetWidth();
	int h6 = img6->GetHeight();

	int w7 = img7->GetWidth();
	int h7 = img7->GetHeight();

	int w8 = img8->GetWidth();
	int h8 = img8->GetHeight();

	int w9 = img9->GetWidth();
	int h9 = img9->GetHeight();

	int w10 = img10->GetWidth();
	int h10 = img10->GetHeight();

	int w11 = img11->GetWidth();
	int h11 = img11->GetHeight();

	for (int i = 0; i < 2; ++i)
		img3->Draw(hdc, 1000, 905 + h3 / 3 * i, w3 - 30, 150, 0, 0, w3, 150);

	img3->Draw(hdc, 1000, 905 + h3 / 3 * 2, w3 - 30, 70, 0, 0, w3, 150);
	img3->Draw(hdc, 1000, 905 + h3 / 3 * 2.5, w3 - 160, 130, 0, 0, w3, 150);
	img3->Draw(hdc, 1000, 905 + h3 / 3 * 3, w3 - 170, 80, 0, 0, w3, 150);
	img3->Draw(hdc, 1000 - w3, 905 + h3 / 3 * 3, w3, 150, 0, 0, w3, 150);
	img3->Draw(hdc, 1000 - w3, 905 + h3 / 3 * 3 + 100, w3, 140, 0, 0, w3, 150);

	for (int i = 0; i < 6; ++i)
		img5->Draw(hdc, i * (w - 10), 100, w * 1.09, h, 0, 310, w, 350);//312 365   270  310                   


	//���� �ܵ��
	for (int i = 0; i < 3; ++i)
	{
		img->Draw(hdc, w * i, 183, w, h / 4 * 3 + 183, 0, 0, w, h / 4 * 3);

		for (int j = 0; j < 2; ++j)
		{
			img3->Draw(hdc, (w - 10) * i, 183 + h + j * h3, w, 200, 0, 0, w, 200);
			img3->Draw(hdc, (w - 10) * i, 183 + h + 200 + h3 * j, w, h3 - 200, 0, 0, w, 200);
		}
	}
	img3->Draw(hdc, (w - 10) * 0, 183 + h + 3 * h3-415, w+55, 140, 0, 0, w, 140);
	img3->Draw(hdc, (w - 10) * 1, 183 + h + 3 * h3-415, w+55, 140, 0, 0, w, 140);
	
	img4->Draw(hdc, w * 3, 180, w, h, 0, 0, 250, 210);//270 380
	img4->Draw(hdc, w * 3, 180 + h, w, h, 0, 0, 250, 210);//270 380


	for (int i = 0; i < 2; ++i)
	{
		img3->Draw(hdc, w * 3 + w + (w - 40) * i, 180, w, h, 130, 0, 190, 140);//�� �ܵ��
		img3->Draw(hdc, w * 3 + w + (w - 40) * i, 180 + h, w, h, 130, 0, 190, 140);//�� �ܵ�� �ؿ�
	}

	img3->Draw(hdc, (w - 10) * 3, 183 + h, w * 0.25, h, 0, 0, 50, h);//������

	img->Draw(hdc, w * 3 + w + 170, 180, w, h / 4 * 3, 0, 0, w, h / 4 * 3);
	img->Draw(hdc, w * 3 + w + 170 + w, 180, w / 3, h / 4 * 3, 0, 0, w / 3, h / 4 * 3);

	for (int i = 0; i < 2; ++i)
		img4->Draw(hdc, w * 3 + w + 170 + w + 110, 102 + h4 * i, 20, h, 0, 0, 20, h);
	img4->Draw(hdc, w * 3 + w + 170 + w + 110, 70 + h4 * 2, 20, h, 0, 0, 20, h);


	img9->TransparentBlt(hdc, w * 3 + w + 120 + w, 180 + w / 2, 146, 292, 0, 0, 146, 292, RGB(255, 0, 255)); // ������

	for (int i = 0; i < 2; ++i)
		img8->Draw(hdc, 1280 + w8 * i, 1010, w8, h8, 0, 0, w8, h8);


	img10->Draw(hdc, 0, 900, w10, h10, 0, 200, 200, h10);//-250 200-
	img10->Draw(hdc, 200, 900, w10, h10, 30, 200, 150, h10);//-250 200-
	img10->Draw(hdc, 200 + w10, 900, w10, h10, 30, 200, 150, h10);//-250 200-
	img10->Draw(hdc, 100 + w10 * 2, 900, w10 / 2, h10, 30, 200, 200, h10);//-250 200-

	for (int i = 0; i < 3; ++i)
	{
		img->Draw(hdc, i * w, 900 + h10 / 2 - 40, w + 30, h / 4 * 3, 0, 0, w, h / 4 * 3);
		img->Draw(hdc, i * w, 900 + h10 / 2 - 40, w / 3 + 30, h / 4 * 3, 0, 0, w / 3, h / 4 * 3);
	}

	img4->Draw(hdc, 1280, 1010, 20, h / 4 * 3, 0, 0, 20, h);
	img8->Draw(hdc, 1140, 910 + h4, w8 / 2, h8, 0, 0, w8 / 2, h8);
	img4->Draw(hdc, 1140, 910 + h4, 20, h / 8 * 3, 0, 0, 20, h / 2);
	img8->Draw(hdc, 1000, 905 + h4 + h4 / 2, w8 / 2, h8, 0, 0, w8 / 2, h8);
	img4->Draw(hdc, 1000, 905 + h4 + h4 / 2, 20, h / 8 * 3, 0, 0, 20, h / 2);

	for (int i = 0; i < 7; ++i)
		img8->Draw(hdc, 860 - i * w8 / 2, 900 + h4 * 1.5 + h4 / 2, w8 / 2, h8, 0, 0, w8 / 2, h8);

	//���� ����
	for (int i = 0; i < 4; ++i)
		img4->Draw(hdc, w * 3 + w + 170 + w + 110+400, 102 + h4 * i+200, 20, h, 0, 0, 20, h);
	
	//�� ����
		img5->Draw(hdc, w * 3 + w + 170 + w + 110 + 400+20, 300, w * 1.09, h, 0, 310, w, 350);//312 365   270  310  
		img5->Draw(hdc, w * 3 + w + 170 + w + 110 + 400 + 20+340, 300, w * 1.09-50, h, 0, 310, w-50, 350);//312 365   270  310  
	//�����
	for(int i=0;i<2;++i)
		img->Draw(hdc, w * 3 + w + 170 + w + 110 + 400 + 20 + i * w5, 300+80, w, h / 4 * 3 + 183, 0, 0, w, h / 4 * 3);
	//�ܵ��
	for(int i=0;i<2;++i)
		for(int j=0;j<5;++j)
			img3->Draw(hdc, 2320+w3*i, 800+150*j, w3+10, 150, 0, 0, w3, 150);
	//������ ����
	for (int i = 0; i < 4; ++i)
		img4->Draw(hdc, w * 3 + w + 170 + w + 110 + 400 + 660, 102 + h4 * i + 200, 20, h, 0, 0, 20, h);
	//��Ÿ��
	/*img8->Draw(hdc, 2420, 1000, w8 / 2, h8, 0, 0, w8 / 2, h8);
	img8->Draw(hdc, 2420+w8/2, 1000, w8 / 2, h8, 0, 0, w8 / 2, h8);
	img4->Draw(hdc, 2420, 1020 , 20, h / 8 * 3, 0, 0, 20, h / 2);
	img4->Draw(hdc, 2420+w8-20, 1020, 20, h / 8 * 3, 0, 0, 20, h / 2);*/
	img10->Draw(hdc, 2420 , 1000, w10, h10, 0, 200, 250, h10);//-250 200-
	/*for (int i = 0; i < 3; ++i)
	{
		img->Draw(hdc, i * w, 900 + h10 / 2 - 40, w + 30, h / 4 * 3, 0, 0, w, h / 4 * 3);
		img->Draw(hdc, i * w, 900 + h10 / 2 - 40, w / 3 + 30, h / 4 * 3, 0, 0, w / 3, h / 4 * 3);
	}*/
	img->Draw(hdc, 2420,1000+h10-200, w + 30, h / 4 * 3, 0, 0, w, h / 4 * 3);
	//��ġ
	img11->TransparentBlt(hdc, w * 3 + w + 170 + w + 110 + 400 +30, 102 + h4 + 400, w11, h11, 0, 0, w11, h11, RGB(255, 0, 255));
	//img11->Draw(hdc, w * 3 + w + 170 + w + 110 + 400+100, 102 + h4 + 200, w11, h11, 0, 0, w11, h11);
	//��� ���� ���ڶ�����
	img3->Draw(hdc, 2600, 2500, w3 , h3, 0, 0, w3, h3);
}

void animation(HDC hdc, CImage* img, const Effect& ch, ELEMENT type)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	if (EL_ICE == type)
	{
		img->TransparentBlt(hdc, ch.posX, ch.posY, ch.sizeX, ch.sizeY,
			ch.sizeX * (ch.animPosX - 1), ch.sizeY * (ch.animPosY - 1), ch.sizeX, ch.sizeY, RGB(200, 230, 250));
	}
	if (EL_ICE_END == type)
	{
		img->TransparentBlt(hdc, ch.posX, ch.posY, ch.sizeX, ch.sizeY,
			ch.sizeX * (ch.animPosX - 1), ch.sizeY * (ch.animPosY - 1), ch.sizeX, ch.sizeY, RGB(200, 230, 250));
	}
}

void animation(HDC hdc, CImage* img, Character* ch, TYPE type)
{
	int w = img->GetWidth();
	int h = img->GetHeight();

	if (TYPE_EFFECT == type)
	{
		if (DIR_LEFT == ch->dir)
			img->TransparentBlt(hdc, ch->posX - 80, ch->posY, ch->ef_sizeX, ch->ef_sizeY,
				ch->ef_sizeX * (ch->ef_animPosX - 1), ch->ef_sizeY * (ch->ef_animPosY - 1), ch->ef_sizeX, ch->ef_sizeY, RGB(255, 0, 255));
		if (DIR_RIGHT == ch->dir)
			img->TransparentBlt(hdc, ch->posX + 80, ch->posY, ch->ef_sizeX, ch->ef_sizeY,
				ch->ef_sizeX * (ch->ef_animPosX - 1), ch->ef_sizeY * (ch->ef_animPosY - 1), ch->ef_sizeX, ch->ef_sizeY, RGB(255, 0, 255));
	}
	else
	{
		img->TransparentBlt(hdc, ch->posX, ch->posY, ch->sizeX, ch->sizeY,
			ch->sizeX * (ch->animPosX - 1), ch->sizeY * (ch->animPosY - 1), ch->sizeX, ch->sizeY, RGB(255, 0, 255));
	}
}

void cal_movement(DIR* dir, int* posx, int* posy, bool* input, bool* idle)
{
	POINT move;

	if (input[VK_LEFT] == input[VK_RIGHT])
		move.x = 0.f;
	else if (input[VK_LEFT])
	{
		*dir = DIR_LEFT;
		move.x = -15.f;
	}
	else
	{
		*dir = DIR_RIGHT;
		move.x = 15.f;
	}


	if (input[VK_UP] == input[VK_DOWN])
		move.y = 0.f;
	else if (input[VK_UP])
	{
		*dir = DIR_UP;
		move.y = -15.f;
	}
	else
	{
		*dir = DIR_DOWN;
		move.y = 15.f;
	}

	*posx += move.x;
	*posy += move.y;
}

DIR check_collision(Character* a, Character* b)
{
	RECT a_rect = { a->posX,a->posY ,a->posX + a->sizeX ,a->posY + a->sizeY }; // �и��� ��
	RECT b_rect = { b->posX,b->posY ,b->posX + b->sizeX ,b->posY + b->sizeY }; // �̴� ��
	int a_centerX = a->posX + a->sizeX / 2;
	int a_centerY = a->posY + a->sizeY / 2;
	int b_centerX = b->posX + b->sizeX / 2;
	int b_centerY = b->posY + b->sizeY / 2;
	RECT temp;
	int push_x, push_y;
	if (IntersectRect(&temp, &a_rect, &b_rect)) // a,b�� �浹�� �߻��ϸ�
	{
		push_x = temp.right - temp.left; // �浹 ������ ���� ũ��
		push_y = temp.bottom - temp.top; // �浹 ������ ���� ũ��

		if (push_x > push_y) // �浹 �簢���� ����>���� �϶�= ��or�Ʒ� �浹
		{
			if (a_centerY > b_centerY) // �̴¾ְ� �и��¾� ���� ������
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posY += push_y / 4;
				return DIR_DOWN;
			}
			else // �̴¾ְ� �и��¾� �Ʒ��� ������
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posY -= push_y / 4;
				return DIR_UP;
			}
		}
		else // �浹 �簢���� ����<���� �϶�= ��or�� �浹
		{
			if (a_centerX > b_centerX) // �̴¾ְ� �и��¾� ���ʿ� ������
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posX += push_x / 4;
				return DIR_RIGHT;
			}
			else // �̴¾ְ� �и��¾� �����ʿ� ������
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posX -= push_x / 4;
				return DIR_LEFT;
			}
		}
	}
}

DIR check_collision(Character* a, Effect* b)
{
	RECT a_rect = { a->posX,a->posY ,a->posX + a->sizeX ,a->posY + a->sizeY }; // �и��� ��
	RECT b_rect = { b->posX,b->posY ,b->posX + b->sizeX ,b->posY + b->sizeY }; // �̴� ��
	int a_centerX = a->posX + a->sizeX / 2;
	int a_centerY = a->posY + a->sizeY / 2;
	int b_centerX = b->posX + b->sizeX / 2;
	int b_centerY = b->posY + b->sizeY / 2;
	RECT temp;
	int push_x, push_y;
	if (IntersectRect(&temp, &a_rect, &b_rect)) // a,b�� �浹�� �߻��ϸ�
	{
		push_x = temp.right - temp.left; // �浹 ������ ���� ũ��
		push_y = temp.bottom - temp.top; // �浹 ������ ���� ũ��

		if (push_x > push_y) // �浹 �簢���� ����>���� �϶�= ��or�Ʒ� �浹
		{
			if (a_centerY > b_centerY) // �̴¾ְ� �и��¾� ���� ������
			{
				if (TYPE_SWORD == a->type)
				{
					b->animPosY = 4;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posY += push_y / 4;
				return DIR_DOWN;
			}
			else // �̴¾ְ� �и��¾� �Ʒ��� ������
			{
				if (TYPE_SWORD == a->type)
				{
					b->animPosY = 4;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posY -= push_y / 4;
				return DIR_UP;
			}
		}
		else // �浹 �簢���� ����<���� �϶�= ��or�� �浹
		{
			if (a_centerX > b_centerX) // �̴¾ְ� �и��¾� ���ʿ� ������
			{
				if (TYPE_SWORD == a->type)
				{
					b->animPosY = 4;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posX += push_x / 4;
				return DIR_RIGHT;
			}
			else // �̴¾ְ� �и��¾� �����ʿ� ������
			{
				if (TYPE_SWORD == a->type)
				{
					b->animPosY = 4;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
				}
				a->posX -= push_x / 4;
				return DIR_LEFT;
			}
		}
	}
}