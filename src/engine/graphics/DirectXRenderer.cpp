#ifdef _WIN32

#include "DirectXRenderer.h"
#include "../platform/window.h"
#include <iostream>

DirectXRenderer::DirectXRenderer() : valid_(false) {
    memset(&context_, 0, sizeof(context_));
}

DirectXRenderer::~DirectXRenderer() {
    Shutdown();
}

bool DirectXRenderer::Initialize(int width, int height, bool fullscreen) {
    if (valid_) {
        return true;
    }

    context_.width = width;
    context_.height = height;
    context_.fullscreen = fullscreen;

    context_.hwnd = CreateMainWindow(width, height, fullscreen);
    if (!context_.hwnd) {
        std::cerr << "Failed to create window for DirectX renderer!" << std::endl;
        return false;
    }

    if (!CreateDevice(context_.hwnd)) {
        std::cerr << "Failed to create DirectX device!" << std::endl;
        return false;
    }

    if (!CreateSprite()) {
        std::cerr << "Failed to create DirectX sprite!" << std::endl;
        return false;
    }

    valid_ = true;
    return true;
}

void DirectXRenderer::Shutdown() {
    ReleaseResources();
    valid_ = false;
}

bool DirectXRenderer::BeginFrame() {
    if (!valid_ || !context_.device && !context_.deviceEx) {
        return false;
    }

    IDirect3DDevice9* device = context_.deviceEx ? context_.deviceEx : context_.device;
    if (FAILED(device->BeginScene())) {
        return false;
    }

    return true;
}

void DirectXRenderer::EndFrame() {
    if (!valid_ || !context_.device && !context_.deviceEx) {
        return;
    }

    IDirect3DDevice9* device = context_.deviceEx ? context_.deviceEx : context_.device;
    device->EndScene();
    device->Present(nullptr, nullptr, nullptr, nullptr);
}

void DirectXRenderer::Clear(float r, float g, float b, float a) {
    if (!valid_ || !context_.device && !context_.deviceEx) {
        return;
    }

    IDirect3DDevice9* device = context_.deviceEx ? context_.deviceEx : context_.device;
    DWORD color = D3DCOLOR_ARGB(
        static_cast<BYTE>(a * 255.0f),
        static_cast<BYTE>(r * 255.0f),
        static_cast<BYTE>(g * 255.0f),
        static_cast<BYTE>(b * 255.0f)
    );
    device->Clear(0, nullptr, D3DCLEAR_TARGET, color, 1.0f, 0);
}

bool DirectXRenderer::IsValid() const {
    return valid_;
}

void* DirectXRenderer::GetNativeContext() {
    return &context_;
}

const char* DirectXRenderer::GetName() const {
    return "DirectX 9";
}

bool DirectXRenderer::CreateDevice(HWND hwnd) {
    context_.d3d9Dll = LoadLibraryA("d3d9.dll");
    if (!context_.d3d9Dll) {
        std::cerr << "Failed to load d3d9.dll" << std::endl;
        return false;
    }

    typedef HRESULT(WINAPI* Direct3DCreate9ExFunc)(UINT, IDirect3D9Ex**);
    Direct3DCreate9ExFunc Direct3DCreate9Ex = (Direct3DCreate9ExFunc)GetProcAddress(context_.d3d9Dll, "Direct3DCreate9Ex");

    if (Direct3DCreate9Ex) {
        if (SUCCEEDED(Direct3DCreate9Ex(D3D_SDK_VERSION, &context_.d3d9ex))) {
            context_.d3d9 = context_.d3d9ex;
        }
    }

    if (!context_.d3d9) {
        typedef IDirect3D9* (WINAPI* Direct3DCreate9Func)(UINT);
        Direct3DCreate9Func Direct3DCreate9 = (Direct3DCreate9Func)GetProcAddress(context_.d3d9Dll, "Direct3DCreate9");
        if (Direct3DCreate9) {
            context_.d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        }
    }

    if (!context_.d3d9) {
        std::cerr << "Failed to create Direct3D9 object" << std::endl;
        return false;
    }

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = !context_.fullscreen;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hwnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = context_.width;
    d3dpp.BackBufferHeight = context_.height;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.EnableAutoDepthStencil = FALSE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (context_.d3d9ex) {
        if (FAILED(context_.d3d9ex->CreateDeviceEx(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, nullptr, &context_.deviceEx))) {
            std::cerr << "Failed to create Direct3D9Ex device" << std::endl;
            return false;
        }
    } else {
        if (FAILED(context_.d3d9->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &context_.device))) {
            std::cerr << "Failed to create Direct3D9 device" << std::endl;
            return false;
        }
    }

    return true;
}

bool DirectXRenderer::CreateSprite() {
    if (!context_.device && !context_.deviceEx) {
        return false;
    }

    IDirect3DDevice9* device = context_.deviceEx ? context_.deviceEx : context_.device;
    
    HMODULE d3dx9Dll = LoadLibraryA("d3dx9_43.dll");
    if (!d3dx9Dll) {
        d3dx9Dll = LoadLibraryA("d3dx9_42.dll");
    }
    if (!d3dx9Dll) {
        d3dx9Dll = LoadLibraryA("d3dx9_41.dll");
    }
    if (!d3dx9Dll) {
        d3dx9Dll = LoadLibraryA("d3dx9_40.dll");
    }
    if (!d3dx9Dll) {
        d3dx9Dll = LoadLibraryA("d3dx9.dll");
    }

    if (d3dx9Dll) {
        typedef HRESULT(WINAPI* D3DXCreateSpriteFunc)(LPDIRECT3DDEVICE9, LPD3DXSPRITE*);
        D3DXCreateSpriteFunc D3DXCreateSprite = (D3DXCreateSpriteFunc)GetProcAddress(d3dx9Dll, "D3DXCreateSprite");
        if (D3DXCreateSprite) {
            if (SUCCEEDED(D3DXCreateSprite(device, &context_.sprite))) {
                return true;
            }
        }
    }

    std::cout << "D3DX not available, sprite functionality disabled" << std::endl;
    return true;
}

void DirectXRenderer::ReleaseResources() {
    if (context_.sprite) {
        context_.sprite->Release();
        context_.sprite = nullptr;
    }
    if (context_.deviceEx) {
        context_.deviceEx->Release();
        context_.deviceEx = nullptr;
    }
    if (context_.device) {
        context_.device->Release();
        context_.device = nullptr;
    }
    if (context_.d3d9ex) {
        context_.d3d9ex->Release();
        context_.d3d9ex = nullptr;
    }
    if (context_.d3d9) {
        context_.d3d9->Release();
        context_.d3d9 = nullptr;
    }
    if (context_.d3d9Dll) {
        FreeLibrary(context_.d3d9Dll);
        context_.d3d9Dll = nullptr;
    }
}

#endif 