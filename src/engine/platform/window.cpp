#ifdef _WIN32
#include "window.h"
#include <iostream>
#include <cwchar>

wchar_t unk_1685BFC[1] = { 0 };

const char* sub_D588F0(int* a1, const char* format, ...) {
    static char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    return buf;
}

void sub_D58020(void* this_, int a2 = 0) {}

LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

HWND CreateMainWindow(int width, int height, bool fullscreen) {
    const wchar_t* className = L"RaymanLegendsWindowClass";
    const wchar_t* windowTitle = L"Rayman Legends";
    HMODULE ModuleHandleW;
    HWND Window;
    RECT Rect;
    int v22 = 0;

    WNDCLASSW WndClass = {};
    WndClass.style = 3;
    WndClass.lpfnWndProc = SimpleWndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = GetModuleHandleW(0);
    WndClass.hIcon = LoadIconW(WndClass.hInstance, (LPCWSTR)0x66);
    WndClass.hCursor = LoadCursorW(0, (LPCWSTR)0x7F00);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(4);
    WndClass.lpszMenuName = 0;
    WndClass.lpszClassName = className;

    if (!RegisterClassW(&WndClass)) {
        DWORD err = GetLastError();
        std::wcerr << L"RegisterClassW failed: " << err << std::endl;
        sub_D588F0(&v22, "Failed to register window class : %ls", className);
        MessageBoxW(0, className, L"RegisterClassW Failed", 0x10u);
        sub_D58020(&v22);
        return nullptr;
    }

    Rect.left = 0;
    Rect.top = 0;
    Rect.right = width;
    Rect.bottom = height;
    AdjustWindowRect(&Rect, 0xCA0000u, 0);
    ModuleHandleW = GetModuleHandleW(0);
    Window = CreateWindowExW(
        0,
        className,
        windowTitle,
        fullscreen ? 0x80000000 : 13238272,
        0,
        0,
        Rect.right - Rect.left,
        Rect.bottom - Rect.top,
        0,
        0,
        ModuleHandleW,
        0
    );
    if (!Window) {
        DWORD err = GetLastError();
        std::wcerr << L"CreateWindowExW failed: " << err << std::endl;
        sub_D588F0(&v22, "CreateWindow Failed !");
        MessageBoxW(0, windowTitle, L"CreateWindow Failed", 0x10u);
        sub_D58020(&v22);
        return nullptr;
    }
    ShowWindow(Window, TRUE);
    UpdateWindow(Window);
    return Window;
}
#endif