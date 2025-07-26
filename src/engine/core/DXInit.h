#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

struct DX9Context {
    HMODULE d3d9Dll = nullptr;
    IDirect3D9* d3d9 = nullptr;
    IDirect3DDevice9* device = nullptr;
    IDirect3D9Ex* d3d9ex = nullptr;
    IDirect3DDevice9Ex* deviceEx = nullptr;
    LPD3DXSPRITE sprite = nullptr;
    bool usingEx = false;
};

bool InitDirect3D9(DX9Context& ctx, HWND hwnd, int width, int height, bool fullscreen);