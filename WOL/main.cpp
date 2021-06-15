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

	while (1) 
	{
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT)
				break;
			TranslateMessage(&Message); 
			DispatchMessage(&Message);
		}
		else 
		{
			//count++; 
			//wsprintf(str, _T("현재 카운터는 %d입니다"), count); 
			//TextOut(hdc, 10, 10, str, lstrlen(str));
		}
	}

	return Message.wParam;
}

void animation(HDC hdc, CImage* img, Character* ch, TYPE type);
void animation(HDC hdc, CImage* img, const Effect& ch, ELEMENT type);
void cal_movement(DIR* dir, int* posx, int* posy, bool* input, const int& speed);
void set_obstacle(MapTile(*map)[25], MAP stage);
void total_boundary_correction(const int& mapx, const int& mapy, int* posx, int* posy, Character* ch);
void check_collision(Character* a, Character* b);
bool check_collision(Character* a, Effect* b);
DIR check_collision(Character* a, MapTile(*b)[25]);

void Sound_Setup() 
{
	FMOD_System_Create(&System); 
	FMOD_System_Init(System, CH_END, FMOD_INIT_NORMAL, NULL);

	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\DUNGEON_BGM.mp3", FMOD_LOOP_NORMAL, 0, &bgmSound[0]);

	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\ICE_BLAST_3.mp3", FMOD_DEFAULT, 0, &effectSound[EF_ICEPOP]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\ICE_KRYSTAL_START.mp3", FMOD_DEFAULT, 0, &effectSound[EF_ICESHOOT]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\FIRE_DRAGON_3.mp3", FMOD_DEFAULT, 0, &effectSound[EF_FIRESHOOT]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\ULT_USE.mp3", FMOD_DEFAULT, 0, &effectSound[EF_FIREPOP]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\SWORDMAN_ATTACK.mp3", FMOD_DEFAULT, 0, &effectSound[EF_MONSTERATTACK]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\SWORDMAN_RUN_2.mp3", FMOD_DEFAULT, 0, &effectSound[EF_MONSTERMOVE]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\ENEMY_HITTED_ICE_1.mp3", FMOD_DEFAULT, 0, &effectSound[EF_MONSTERHIT]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\SWORDMAN_RUN_1.mp3", FMOD_DEFAULT, 0, &effectSound[EF_PLAYERWALK]);
	FMOD_System_CreateSound(System, "WOL_RESOURCE\\Sound\\HIT_SOUND_NORMAL_1.mp3", FMOD_DEFAULT, 0, &effectSound[EF_PLAYERHIT]);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, memdc;
	static RECT c;
	static MapTile map1tile[25][25], map2tile[25][25], bossMaptile[25][25];
	static POINT mouse, camera;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	static bool keyLayout[256];

	static CImage Logo, Target, Profile, Health;
	static CImage Stage1Map, Stage2Map,BossMap, statue, chairLeft;
	static CImage treeLeft, treeRight, treeLeftPurple, treeRightPurple, insignia;
	static CImage PlayerFront, PlayerBack, PlayerLeft, PlayerRight;
	static CImage ArcherBowLeft, ArcherBowRight, ArcherLeft, ArcherRight; // 몬스터1
	static CImage SwordmanLeft, SwordmanRight, SwordmanAttack; // 몬스터3
	static CImage FireAttack, FireParticle, IceCard, IceAttack, IceParticle, Wind; // 이펙트
	static CImage bossMap, stage1;
	static Character pl, sw, ar, wz, bs; // 플레이어,소드맨,아처,위자드,보스
	static SCENE sceneNow;
	static MAP mapNow;
	HBITMAP hBitmap;
	static vector<Effect> ice, ice_end, fire, fire_end;

	static bool isIdle, isPlayerAttack, isCooltime;
	static int whereToGo = 4;
	static int howManyMove = 0;
	static int winposX, winposY, centerX, centerY, mapX, mapY, mapTileX, mapTileY;
	static short speed_anim, speed_move, speed_attack;
	switch (uMsg)
	{
	case WM_CREATE: // 첫 초기화
	{
		// UI
		Logo.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\READY_MENU.bmp");
		Target.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\UI_MOUSE.bmp");
		Profile.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\UI_PLAYERBAR.bmp");
		Health.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\UI_HPBAR.bmp");
		Sound_Setup();
		FMOD_System_PlaySound(System, bgmSound[0], NULL, 0, &Channel[CH_BACK]);
		GetClientRect(hWnd, &c);
		ShowCursor(false);
		sceneNow = SCENE_LOGO;
		speed_anim = 20;
		speed_move = 40;
		speed_attack = 250;
		isCooltime = false;
		mapX = 2700;
		mapY = 2600;
		mapTileX = 25;
		mapTileY = 25;
		mapNow = M_MAP1;

		// Map
		Stage1Map.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\stage1.bmp");
		Stage2Map.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\stage2.bmp");
		BossMap.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Map\\bossMap.png");
		for (int i = 0; i < mapTileX; ++i)
		{
			for (int j = 0; j < mapTileY; ++j)
			{
				map1tile[i][j].m = { i * mapX / mapTileX,j * mapY / mapTileY,(i + 1) * mapX / mapTileX,(j + 1) * mapY / mapTileY };
				map1tile[i][j].isObs = false;
				map2tile[i][j].m = { i * mapX / mapTileX,j * mapY / mapTileY,(i + 1) * mapX / mapTileX,(j + 1) * mapY / mapTileY };
				map2tile[i][j].isObs = false;
				bossMaptile[i][j].m = { i * mapX / mapTileX,j * mapY / mapTileY,(i + 1) * mapX / mapTileX,(j + 1) * mapY / mapTileY };
				bossMaptile[i][j].isObs = false;
			}
		}
		set_obstacle(map1tile, M_MAP1);
		set_obstacle(map2tile, M_MAP2);
		set_obstacle(bossMaptile, M_BOSS);

		// Player
		PlayerFront.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\FRONT_COMPLETE.bmp");
		PlayerBack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\BACK_COMPLETE.bmp");
		PlayerLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\LEFT_COMPLETE.bmp");
		PlayerRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Player\\RIGHT_COMPLETE.bmp");
		pl.posX = 1500, pl.posY = 1500, pl.animPosX = 1, pl.animPosY = 2;
		pl.sizeX = 180, pl.sizeY = 182, pl.hp = 100, pl.moveSpeed = 15;
		pl.dir = DIR_DOWN, pl.type = TYPE_PLAYER, pl.st = ST_IDLE;
		isIdle = true;

		// Monster
		//ArcherBowLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_LEFT.bmp");
		//ArcherBowRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_BOW_RIGHT.bmp");
		//ArcherLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_LEFT.bmp");
		//ArcherRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Archer\\ARCHER_RIGHT.bmp");

		SwordmanLeft.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_LEFT.bmp");
		SwordmanRight.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_RIGHT.bmp");
		SwordmanAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Swordman\\SWORDMAN_ATTACK.bmp");
		sw.posX = 950, sw.posY = 500, sw.animPosX = 1, sw.animPosY = 2;
		sw.sizeX = 200, sw.sizeY = 202, sw.hp = 100, sw.moveSpeed = 2;
		sw.ef_sizeX = 200, sw.ef_sizeY = 200, sw.ef_animPosX = 1;
		sw.type = TYPE_SWORD, sw.st = ST_IDLE;

		// Effect
		IceAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\ICE_CRYSTAL.bmp");
		IceParticle.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\ICE_BLAST.bmp");
		FireAttack.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\fireball.bmp");
		FireParticle.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\FIRE_PARTICLE.bmp");
		//IceCard.Load(L"WOL_RESOURCE\\WOL_TEXTURE\\Element\\ICE_KRYSTAL_CARD.bmp");
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

		case TM_ANIMATION:
		{
			if (1 == pl.animPosY) pl.animPosX = 1;
			if (2 == pl.animPosY)
			{
				if (10 <= pl.animPosX) pl.animPosX = 1;
				else pl.animPosX += 1;
			}
			if (4 == pl.animPosY)
			{
				if (4 <= pl.animPosX)
				{
					pl.animPosX = 1;
					pl.animPosY = 5;
				}
				else pl.animPosX += 1;
			}
			if (5 == pl.animPosY)
			{
				if (4 <= pl.animPosX)
				{
					pl.animPosX = 1;
					pl.animPosY = 4;
				}
				else pl.animPosX += 1;
			}
			if (6 == pl.animPosY)
			{
				if (2 <= pl.animPosX) pl.animPosX = 1;
				else pl.animPosX += 1;
			}
			if (7 == pl.animPosY)
			{
				if (7 <= pl.animPosX) pl.animPosX = 7;
				else pl.animPosX += 1;
			}
			if (0 != ice.size())
			{
				for (auto it = ice.begin(); it != ice.end();)
				{
					if (it->posX == it->endPosX && it->posY == it->endPosY)
					{
						Effect temp = { it->endPosX ,it->endPosY ,it->endPosX ,it->endPosY,
							IceParticle.GetWidth() / 8,IceParticle.GetHeight(),1,1 };
						ice_end.emplace_back(temp);
						it = ice.erase(it);
					}
					else
					{
						it->mid += 0.1f;
						it->posX = it->posX * (1 - it->mid) + it->mid * it->endPosX;
						it->posY = it->posY * (1 - it->mid) + it->mid * it->endPosY;

						if (18 <= it->animPosX) it->animPosX = 1;
						else it->animPosX += 1;
						++it;
					}
				}

				for (int i = 0; i < ice.size(); ++i)
				{
					if (check_collision(&sw, &ice[i]))
					{
						Effect temp = { ice[i].posX ,ice[i].posY ,ice[i].posX ,ice[i].posY,
							IceParticle.GetWidth() / 8,IceParticle.GetHeight(),1,1 };
						ice_end.emplace_back(temp);
						ice.erase(ice.begin() + i);
					}
				}
			}

			if (0 != ice_end.size())
			{
				for (auto it = ice_end.begin(); it != ice_end.end();)
				{
					if (8 <= it->animPosX)
					{
						FMOD_System_PlaySound(System, effectSound[EF_ICEPOP], NULL, 0, &Channel[CH_PARTICLE]);
						it = ice_end.erase(it);
					}
					else
					{
						it->animPosX += 1;
						++it;
					}
				}
			}

			if (0 != fire.size())
			{
				for (auto it = fire.begin(); it != fire.end();)
				{
					if (it->posX == it->endPosX && it->posY == it->endPosY)
					{
						Effect temp = { it->endPosX ,it->endPosY ,it->endPosX ,it->endPosY,
							FireParticle.GetWidth() / 7,FireParticle.GetHeight() / 4,1,3 };
						fire_end.emplace_back(temp);
						it = fire.erase(it);
					}
					else
					{
						it->mid += 0.05f;
						it->posX = it->posX * (1 - it->mid) + it->mid * it->endPosX;
						it->posY = it->posY * (1 - it->mid) + it->mid * it->endPosY;

						if (5 <= it->animPosX) it->animPosX = 1;
						else it->animPosX += 1;
						++it;
					}
				}
				for (int i = 0; i < fire.size(); ++i)
				{
					if (check_collision(&sw, &fire[i]))
					{
						Effect temp = { fire[i].endPosX ,fire[i].endPosY ,fire[i].endPosX ,fire[i].endPosY,
								FireParticle.GetWidth() / 7,FireParticle.GetHeight() / 4,1,3 };
						fire_end.emplace_back(temp);
						fire.erase(fire.begin() + i);
					}
				}
			}
			if (0 != fire_end.size())
			{
				for (auto it = fire_end.begin(); it != fire_end.end();)
				{
					if (7 <= it->animPosX)
					{
						FMOD_System_PlaySound(System, effectSound[EF_FIREPOP], NULL, 0, &Channel[CH_PARTICLE]);
						it = fire_end.erase(it);
					}
					else
					{
						it->animPosX += 1;
						++it;
					}
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		case TM_MOVE:
		{
			if (ST_DEATH != pl.st)
			{
				if (!keyLayout[VK_LEFT] && !keyLayout[VK_UP] && !keyLayout[VK_DOWN] && !keyLayout[VK_RIGHT] && !keyLayout[VK_LBUTTON])
					pl.animPosY = 1;
				cal_movement(&pl.dir, &pl.posX, &pl.posY, keyLayout, pl.moveSpeed);
			}

			centerX = pl.posX + pl.sizeX / 2;
			centerY = pl.posY + pl.sizeY / 2;

			winposX = centerX - WINDOW_WIDTH / 2;
			winposY = centerY - WINDOW_HEIGHT / 2;

			//// 결계 보정
			total_boundary_correction(mapX, mapY, &winposX, &winposY, NULL);
			total_boundary_correction(mapX, mapY, NULL, NULL, &pl);

			if (2 == sw.animPosY) // 왼쪽 이동
			{
				if (6 <= sw.animPosX) sw.animPosX = 1;
				else sw.animPosX += 1;
			}
			else if (3 == sw.animPosY) // 공격
			{
				if (3 <= sw.animPosX)
				{
					sw.animPosX = 1;
					sw.animPosY = 2;
				}
				else sw.animPosX += 1;
			}
			else if (4 == sw.animPosY)
			{
				if (2 <= sw.animPosX)
				{
					sw.animPosX = 1;
					sw.animPosY = 2;
				}
				else sw.animPosX += 1;
			}
			if (4 <= sw.ef_animPosX) sw.ef_animPosX = 1;
			else sw.ef_animPosX += 1;

			if (pl.posX < sw.posX)
			{
				sw.dir = DIR_LEFT;
				sw.posX -= sw.moveSpeed;
				//FMOD_System_PlaySound(System, effectSound[EF_MONSTERMOVE], NULL, 0, &Channel[CH_MONSTER]);
			}
			else if (pl.posX > sw.posX)
			{
				sw.dir = DIR_RIGHT;
				sw.posX += sw.moveSpeed;
				//FMOD_System_PlaySound(System, effectSound[EF_MONSTERMOVE], NULL, 0, &Channel[CH_MONSTER]);
			}

			if (pl.posY < sw.posY)
			{
				sw.posY -= sw.moveSpeed;
				//FMOD_System_PlaySound(System, effectSound[EF_MONSTERMOVE], NULL, 0, &Channel[CH_MONSTER]);
			}
			else if (pl.posY > sw.posY)
			{
				sw.posY += sw.moveSpeed;
				//FMOD_System_PlaySound(System, effectSound[EF_MONSTERMOVE], NULL, 0, &Channel[CH_MONSTER]);
			}

			if (ST_DEATH != pl.st)
				check_collision(&pl, &sw);
			if (M_MAP1 == mapNow)
			{
				check_collision(&pl, map1tile);
				check_collision(&sw, map1tile);
			}
		}
		break;
		case TM_ATTACK:
		{
			isCooltime = false;
			if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && !isCooltime && ST_DEATH != pl.st)
			{
				int realMouseX = winposX + mouse.x;
				int realMouseY = winposY + mouse.y;
				if (EL_ICE == pl.el)
				{
					int w = IceAttack.GetWidth();
					int h = IceAttack.GetHeight();
					int start = rand() % 300 - 150;
					Effect temp = { pl.posX + start,pl.posY + start,realMouseX ,realMouseY,w / 18,h, 1, 1, 0, EL_ICE };
					ice.emplace_back(temp);
					FMOD_System_PlaySound(System, effectSound[EF_ICESHOOT], NULL, 0, &Channel[CH_PLAYER]);
				}
				if (EL_FIRE == pl.el)
				{
					int w = FireAttack.GetWidth();
					int h = FireAttack.GetHeight();
					int start = rand() % 300 - 150;
					Effect temp = { pl.posX + start,pl.posY + start,realMouseX ,realMouseY,w / 5,h / 2, 1, 1, 0, EL_FIRE };
					fire.emplace_back(temp);
					FMOD_System_PlaySound(System, effectSound[EF_FIRESHOOT], NULL, 0, &Channel[CH_PLAYER]);
				}
				isCooltime = true;
			}
		}
		break;
		case TM_PL_ATTACK:
		{

		}
		break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	break;
	case WM_LBUTTONDOWN:
	{
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
		isPlayerAttack = true;
		pl.animPosY = 4;
		pl.moveSpeed = 5;
		keyLayout[VK_LBUTTON] = 1;
	}
	InvalidateRect(hWnd, NULL, FALSE);
	break;
	case WM_LBUTTONUP:
	{
		isPlayerAttack = false;
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
		keyLayout[VK_LBUTTON] = 0;
		pl.moveSpeed = 15;
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
				SetTimer(hWnd, TM_ANIMATION, speed_anim, NULL);
				SetTimer(hWnd, TM_MOVE, speed_move, NULL);
				SetTimer(hWnd, TM_ATTACK, speed_attack, NULL);
				//Logo.ReleaseDC();
			}
		}

		if (ST_DEATH != pl.st)
		{
			if ('a' == wParam || 'A' == wParam)
			{
				keyLayout[VK_LEFT] = 1;
				if (!isPlayerAttack)
					pl.animPosY = 2;
				//check_collision(&pl, &sw);
			}
			if ('d' == wParam || 'D' == wParam)
			{
				keyLayout[VK_RIGHT] = 1;
				if (!isPlayerAttack)
					pl.animPosY = 2;
				//check_collision(&pl, &sw);
			}
			if ('w' == wParam || 'W' == wParam)
			{
				keyLayout[VK_UP] = 1;
				if (!isPlayerAttack)
					pl.animPosY = 2;
				//check_collision(&pl, &sw);
			}
			if ('s' == wParam || 'S' == wParam)
			{
				keyLayout[VK_DOWN] = 1;
				if (!isPlayerAttack)
					pl.animPosY = 2;
				//check_collision(&pl, &sw);
			}
			if ('1' == wParam)
			{
				pl.el = EL_ICE;
			}
			if ('2' == wParam)
			{
				pl.el = EL_FIRE;
			}
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

		hBitmap = CreateCompatibleBitmap(hdc, mapX, mapY);
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap);

		if (SCENE_LOGO == sceneNow)
		{
			// 로고
			int w = Logo.GetWidth();
			int h = Logo.GetHeight();
			Logo.Draw(memdc, 0, 0, c.right, c.bottom, 0, 0, w, h);
			BitBlt(hdc, c.left, c.top, c.right, c.bottom, memdc, 0, 0, SRCCOPY);
		}
		else if (SCENE_STAGE == sceneNow)
		{
			// 맵
			if (M_MAP1 == mapNow)
			{
				int w = Stage1Map.GetWidth();
				int h = Stage1Map.GetHeight();
				Stage1Map.Draw(memdc, 0, 0, mapX, mapY, 0, 0, w, h);
			}
			else if (M_MAP2 == mapNow)
			{
				int w = Stage2Map.GetWidth();
				int h = Stage2Map.GetHeight();
				Stage2Map.Draw(memdc, 0, 0, mapX, mapY, 0, 0, w, h);
			}
			else if (M_BOSS == mapNow)
			{
				int w = BossMap.GetWidth();
				int h = BossMap.GetHeight();
				BossMap.Draw(memdc, 0, 0, mapX, mapY, 0, 0, w, h);
			}
			// 몬스터
			if (DIR_LEFT == sw.dir)
			{
				if (3 == sw.animPosY) // 칼 휘두르는 모션 중이면 잔상이펙트도 그려지게
				{
					sw.ef_animPosY = 1;
					animation(memdc, &SwordmanAttack, &sw, TYPE_EFFECT);
				}
				animation(memdc, &SwordmanLeft, &sw, TYPE_SWORD);
			}
			if (DIR_RIGHT == sw.dir)
			{
				if (3 == sw.animPosY) // 칼 휘두르는 모션 중이면 잔상이펙트도 그려지게
				{
					sw.ef_animPosY = 2;
					animation(memdc, &SwordmanAttack, &sw, TYPE_EFFECT);
				}
				animation(memdc, &SwordmanRight, &sw, TYPE_SWORD);
			}
			// 플레이어
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
			// 이펙트, 파티클
			if (0 != ice.size())
			{
				for (const auto& a : ice)
				{
					animation(memdc, &IceAttack, a, EL_ICE);
				}
			}
			if (0 != ice_end.size())
			{
				for (const auto& a : ice_end)
				{
					animation(memdc, &IceParticle, a, EL_ICE_END);
				}
			}
			if (0 != fire.size())
			{
				for (const auto& a : fire)
				{
					animation(memdc, &FireAttack, a, EL_FIRE);
				}
			}
			if (0 != fire_end.size())
			{
				for (const auto& a : fire_end)
				{
					animation(memdc, &FireParticle, a, EL_FIRE_END);
				}
			}
			
			Profile.TransparentBlt(memdc, winposX, winposY + WINDOW_HEIGHT - Profile.GetHeight() * 2, Profile.GetWidth(), Profile.GetHeight(),
				0, 0, Profile.GetWidth(), Profile.GetHeight(), RGB(255, 0, 255)); // 체력표시

			Health.TransparentBlt(memdc, winposX + 76, winposY + WINDOW_HEIGHT - Profile.GetHeight() * 2 + 12, pl.hp * Health.GetWidth() / 100, Health.GetHeight(),
				0, 0, Health.GetWidth(), Health.GetHeight(), RGB(255, 255, 255)); // 체력표시

			Target.TransparentBlt(memdc, winposX + mouse.x - 30, winposY + mouse.y - 30, 60, 60, 0, 0, 60, 60, RGB(255, 0, 255)); // 마우스
			BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memdc, winposX, winposY, SRCCOPY);
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
		Stage1Map.ReleaseDC();
		Stage1Map.ReleaseDC();
		BossMap.ReleaseDC();

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

		for (int i = 0; i < EFFECT_COUNT; i++)
			FMOD_Sound_Release(effectSound[i]);
		for (int i = 0; i < SOUND_COUNT; i++)
			FMOD_Sound_Release(bgmSound[i]);
		FMOD_System_Release(System);
		KillTimer(hWnd, TM_ANIMATION);
		KillTimer(hWnd, TM_ATTACK);
		KillTimer(hWnd, TM_MOVE);
		PostQuitMessage(0);

		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void draw_map(HDC hdc, CImage* img)
{
	int w = img->GetWidth();
	int h = img->GetHeight();
	

	img->Draw(hdc, 0, 0, w, h, 0, 0, w, h);

}

void animation(HDC hdc, CImage* img, const Effect& ch, ELEMENT type)
{
	if (EL_ICE == type)
	{
		img->TransparentBlt(hdc, ch.posX, ch.posY, ch.sizeX, ch.sizeY,
			ch.sizeX * (ch.animPosX - 1), ch.sizeY * (ch.animPosY - 1), ch.sizeX, ch.sizeY, RGB(200, 230, 250));
	}
	if (EL_ICE_END == type)
	{
		img->TransparentBlt(hdc, ch.posX, ch.posY, ch.sizeX, ch.sizeY,
			ch.sizeX * (ch.animPosX - 1), ch.sizeY * (ch.animPosY - 1), ch.sizeX, ch.sizeY, RGB(255, 0, 255));
	}
	if (EL_FIRE == type)
	{
		img->TransparentBlt(hdc, ch.posX, ch.posY, ch.sizeX, ch.sizeY,
			ch.sizeX * (ch.animPosX - 1), ch.sizeY * (ch.animPosY - 1), ch.sizeX, ch.sizeY, RGB(0, 0, 0));
	}
	if (EL_FIRE_END == type)
	{
		img->TransparentBlt(hdc, ch.posX, ch.posY, 2*ch.sizeX/3, 2*ch.sizeY/3,
			ch.sizeX * (ch.animPosX - 1), ch.sizeY * (ch.animPosY - 1), ch.sizeX, ch.sizeY, RGB(23, 23, 23));
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

void cal_movement(DIR* dir, int* posx, int* posy, bool* input, const int& speed)
{
	POINT move;

	if (input[VK_LEFT] == input[VK_RIGHT])
		move.x = 0.f;
	else if (input[VK_LEFT])
	{
		*dir = DIR_LEFT;
		move.x = -speed;
	}
	else
	{
		*dir = DIR_RIGHT;
		move.x = speed;
	}


	if (input[VK_UP] == input[VK_DOWN])
		move.y = 0.f;
	else if (input[VK_UP])
	{
		*dir = DIR_UP;
		move.y = -speed;
	}
	else
	{
		*dir = DIR_DOWN;
		move.y = speed;
	}
	*posx += move.x;
	*posy += move.y;
}

void set_obstacle(MapTile(*map)[25], MAP stage)
{
	if (stage == M_MAP1)
	{
		map[0][3].isObs = true, map[1][3].isObs = true, map[2][3].isObs = true, map[3][3].isObs = true, map[4][3].isObs = true;
		map[5][3].isObs = true, map[6][3].isObs = true, map[7][3].isObs = true, map[8][3].isObs = true, map[9][3].isObs = true;
		map[10][3].isObs = true, map[11][3].isObs = true, map[12][3].isObs = true, map[13][3].isObs = true;
	}
	if (stage == M_MAP2)
	{
	}
	if (stage == M_BOSS)
	{
	}
}

void total_boundary_correction(const int& mapx, const int& mapy,int* posx, int* posy, Character* ch)
{
	if (NULL == ch) // 카메라 경계
	{
		if (*posx < 0)
			*posx = 0;
		if (*posy < 0)
			*posy = 0;
		if (*posx > mapx - WINDOW_WIDTH)
			*posx = mapx - WINDOW_WIDTH;
		if (*posy > mapy - WINDOW_HEIGHT)
			*posy = mapy - WINDOW_HEIGHT;
	}
	else // 캐릭터 경계
	{
		if (ch->posX < 0)
			ch->posX = 0;
		if (ch->posY < 0)
			ch->posY = 0;
		if (ch->posX > mapx - ch->sizeX)
			ch->posX = mapx - ch->sizeX;
		if (ch->posY > mapy - ch->sizeY)
			ch->posY = mapy - ch->sizeY;
	}
}

void check_collision(Character* a, Character* b)
{
	RECT a_rect = { a->posX,a->posY ,a->posX + a->sizeX ,a->posY + a->sizeY }; // 밀리는 애
	RECT b_rect = { b->posX,b->posY ,b->posX + b->sizeX ,b->posY + b->sizeY }; // 미는 애
	int a_centerX = a->posX + a->sizeX / 2;
	int a_centerY = a->posY + a->sizeY / 2;
	int b_centerX = b->posX + b->sizeX / 2;
	int b_centerY = b->posY + b->sizeY / 2;
	RECT temp;
	int push_x, push_y;
	if (IntersectRect(&temp, &a_rect, &b_rect)) // a,b에 충돌이 발생하면
	{
		push_x = temp.right - temp.left; // 충돌 범위의 가로 크기
		push_y = temp.bottom - temp.top; // 충돌 범위의 세로 크기

		if (push_x > push_y) // 충돌 사각형의 가로>세로 일때= 위or아래 충돌
		{
			if (a_centerY > b_centerY) // 미는애가 밀리는애 위에 있을때
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
					a->hp -= 10;
				}
				a->posY += push_y * 5;
			}
			else // 미는애가 밀리는애 아래에 있을때
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
					a->hp -= 10;
				}
				a->posY -= push_y * 5;
			}
		}
		else // 충돌 사각형의 가로<세로 일때= 좌or우 충돌
		{
			if (a_centerX > b_centerX) // 미는애가 밀리는애 왼쪽에 있을때
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
					a->hp -= 10;
				}
				a->posX += push_x * 5;
			}
			else // 미는애가 밀리는애 오른쪽에 있을때
			{
				if (TYPE_SWORD == b->type)
				{
					b->animPosY = 3;
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
					a->hp -= 10;
				}
				a->posX -= push_x * 5;
			}
		}
		if (a->hp <= 0)
		{
			a->dir = DIR_DOWN;
			a->hp = 0;
			a->animPosY = 7;
			a->st = ST_DEATH;
		}
	}
}
bool check_collision(Character* a, Effect* b)
{
	RECT a_rect = { a->posX,a->posY ,a->posX + a->sizeX ,a->posY + a->sizeY }; // 밀리는 애
	RECT b_rect = { b->posX + 50,b->posY + 50 ,b->posX + b->sizeX - 50 ,b->posY + b->sizeY - 50 }; // 미는 애
	int a_centerX = a->posX + a->sizeX / 2;
	int a_centerY = a->posY + a->sizeY / 2;
	int b_centerX = b->posX + b->sizeX / 2;
	int b_centerY = b->posY + b->sizeY / 2;
	RECT temp;
	int push_x, push_y;
	if (IntersectRect(&temp, &a_rect, &b_rect)) // a,b에 충돌이 발생하면
	{
		push_x = temp.right - temp.left; // 충돌 범위의 가로 크기
		push_y = temp.bottom - temp.top; // 충돌 범위의 세로 크기

		if (push_x > push_y) // 충돌 사각형의 가로>세로 일때= 위or아래 충돌
		{
			if (a_centerY > b_centerY) // 미는애가 밀리는애 위에 있을때
			{
				if (TYPE_SWORD == a->type)
				{
					a->animPosY = 4;
					a->animPosX = 1;
					FMOD_System_PlaySound(System, effectSound[EF_MONSTERHIT], NULL, 0, NULL);
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
				}
				a->posY += push_y / 4;
				return true;
			}
			else // 미는애가 밀리는애 아래에 있을때
			{
				if (TYPE_SWORD == a->type)
				{
					a->animPosY = 4;
					a->animPosX = 1;
					FMOD_System_PlaySound(System, effectSound[EF_MONSTERHIT], NULL, 0, NULL);
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
				}
				a->posY -= push_y / 4;
				return true;
			}
		}
		else // 충돌 사각형의 가로<세로 일때= 좌or우 충돌
		{
			if (a_centerX > b_centerX) // 미는애가 밀리는애 왼쪽에 있을때
			{
				if (TYPE_SWORD == a->type)
				{
					a->animPosY = 4;
					a->animPosX = 1;
					FMOD_System_PlaySound(System, effectSound[EF_MONSTERHIT], NULL, 0, NULL);
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
				}
				a->posX += push_x / 4;
				return true;
			}
			else // 미는애가 밀리는애 오른쪽에 있을때
			{
				if (TYPE_SWORD == a->type)
				{
					a->animPosY = 4;
					a->animPosX = 1;
					FMOD_System_PlaySound(System, effectSound[EF_MONSTERHIT], NULL, 0, NULL);
				}
				if (TYPE_PLAYER == a->type)
				{
					a->animPosY = 6;
					FMOD_System_PlaySound(System, effectSound[EF_PLAYERHIT], NULL, 0, &Channel[CH_PLAYER]);
				}
				a->posX -= push_x / 4;
				return true;
			}
		}
		return false;
	}
}
DIR check_collision(Character* a, MapTile(*b)[25])
{
	for (int i = 0; i < 25; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			if (b[i][j].isObs)
			{
				RECT a_rect = { a->posX,a->posY ,a->posX + a->sizeX ,a->posY + a->sizeY }; // 밀리는 애
				RECT b_rect = b[i][j].m; // 미는 애
				int a_centerX = a->posX + a->sizeX / 2;
				int a_centerY = a->posY + a->sizeY / 2;
				int b_centerX = b[i][j].m.left + 108 / 2;
				int b_centerY = b[i][j].m.top + 104 / 2;
				RECT temp;
				int push_x, push_y;
				if (IntersectRect(&temp, &a_rect, &b_rect)) // a,b에 충돌이 발생하면
				{
					push_x = temp.right - temp.left; // 충돌 범위의 가로 크기
					push_y = temp.bottom - temp.top; // 충돌 범위의 세로 크기

					if (push_x > push_y) // 충돌 사각형의 가로>세로 일때= 위or아래 충돌
					{
						if (a_centerY > b_centerY) // 미는애가 밀리는애 위에 있을때
						{
							a->posY += push_y+10;
							return DIR_DOWN;
						}
						else // 미는애가 밀리는애 아래에 있을때
						{
							a->posY -= push_y+10;
							return DIR_UP;
						}
					}
					else // 충돌 사각형의 가로<세로 일때= 좌or우 충돌
					{
						if (a_centerX > b_centerX) // 미는애가 밀리는애 왼쪽에 있을때
						{
							a->posX += push_x+10;
							return DIR_RIGHT;
						}
						else // 미는애가 밀리는애 오른쪽에 있을때
						{
							a->posX -= push_x+10;
							return DIR_LEFT;
						}
					}
				}
			}
		}
	}
}