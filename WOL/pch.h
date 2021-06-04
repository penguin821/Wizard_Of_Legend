#pragma once
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <ctime>
#include <atlImage.h>
//#include "resource.h"
//#pragma comment (lib, "msimg32.lib")
#include "Struct.h"

#define _MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;
