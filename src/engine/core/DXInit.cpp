#include "DXInit.h"
#include <iostream>

bool InitDirect3D9(DX9Context& ctx, HWND hwnd, int width, int height, bool fullscreen) {
    ctx.d3d9Dll = LoadLibraryW(L"d3d9.dll");
    if (!ctx.d3d9Dll) {
        std::cerr << "Failed to load d3d9.dll" << std::endl;
        return false;
    }

    typedef HRESULT (WINAPI *Direct3DCreate9ExFunc)(UINT, IDirect3D9Ex**);
    Direct3DCreate9ExFunc Direct3DCreate9Ex = (Direct3DCreate9ExFunc)GetProcAddress(ctx.d3d9Dll, "Direct3DCreate9Ex");

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = !fullscreen;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.hDeviceWindow = hwnd;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    HRESULT hr = E_FAIL;
    ctx.usingEx = false;

    if (Direct3DCreate9Ex) {
        ctx.usingEx = true;
        IDirect3D9Ex* d3d9ex = nullptr;
        hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9ex);
        if (FAILED(hr) || !d3d9ex) {
            std::cerr << "Direct3DCreate9Ex failed: " << std::hex << hr << std::endl;
            return false;
        }
        ctx.d3d9ex = d3d9ex;
        IDirect3DDevice9Ex* deviceEx = nullptr;
        hr = d3d9ex->CreateDeviceEx(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
            &d3dpp,
            nullptr,
            &deviceEx
        );
        if (FAILED(hr) || !deviceEx) {
            std::cerr << "CreateDeviceEx failed: " << std::hex << hr << std::endl;
            d3d9ex->Release();
            return false;
        }
        ctx.deviceEx = deviceEx;
        ctx.device = deviceEx;
    } else {
        IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d9) {
            std::cerr << "Direct3DCreate9 failed" << std::endl;
            return false;
        }
        ctx.d3d9 = d3d9;
        IDirect3DDevice9* device = nullptr;
        hr = d3d9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
            &d3dpp,
            &device
        );
        if (FAILED(hr) || !device) {
            std::cerr << "CreateDevice failed: " << std::hex << hr << std::endl;
            d3d9->Release();
            return false;
        }
        ctx.device = device;
    }

    LPD3DXSPRITE sprite = nullptr;
    hr = D3DXCreateSprite(ctx.device, &sprite);
    if (FAILED(hr) || !sprite) {
        std::cerr << "D3DXCreateSprite failed: " << std::hex << hr << std::endl;
        if (ctx.device) ctx.device->Release();
        if (ctx.d3d9) ctx.d3d9->Release();
        if (ctx.d3d9ex) ctx.d3d9ex->Release();
        return false;
    }
    ctx.sprite = sprite;
    return true;
}