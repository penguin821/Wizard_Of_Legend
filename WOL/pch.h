#pragma once
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <ctime>
#include <atlImage.h>
//#include "resource.h"
//#pragma comment (lib, "msimg32.lib")
//#pragma comment(lib, "msimg32.lib")

#pragma comment(lib, "msimg32.lib")
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "Struct.h"
#include<random>   
using namespace std;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int>direction(0, 3);

#define _MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);

constexpr int WORLD_WIDTH = 3000;
constexpr int WORLD_HEIGHT = 3000;

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;