#pragma once
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <ctime>
#include <atlImage.h>
#include <random>
#include <fmod.h>

#pragma comment(lib, "msimg32.lib")
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

using namespace std;

#include "Struct.h"

#define _MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

constexpr int SOUND_COUNT = 2;
constexpr int EFFECT_COUNT = EF_END;
constexpr int CHANNEL_COUNT = CH_END;
constexpr int NUM_MONSTER = 6;

FMOD_SYSTEM* System;
FMOD_SOUND* bgmSound[SOUND_COUNT];
FMOD_SOUND* effectSound[EFFECT_COUNT];
FMOD_CHANNEL* Channel[CHANNEL_COUNT];