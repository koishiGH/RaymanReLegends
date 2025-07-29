#pragma once
#ifdef _WIN32
#include <windows.h>

HWND CreateMainWindow(int width, int height, bool fullscreen);
LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif