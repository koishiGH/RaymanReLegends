#pragma once

#include <string>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#endif

#ifdef _WIN32
struct DX9Context {
    HMODULE d3d9Dll = nullptr;
    IDirect3D9* d3d9 = nullptr;
    IDirect3D9Ex* d3d9ex = nullptr;
    IDirect3DDevice9* device = nullptr;
    IDirect3DDevice9Ex* deviceEx = nullptr;
    ID3DXSprite* sprite = nullptr;
    HWND hwnd = nullptr;
    int width = 0;
    int height = 0;
    bool fullscreen = false;
};
#endif

struct SDL2Context {
    void* window = nullptr;
    void* renderer = nullptr;
    void* texture = nullptr;
    int width = 0;
    int height = 0;
    bool fullscreen = false;
};

class IGraphicsRenderer {
public:
    virtual ~IGraphicsRenderer() = default;
    virtual bool Initialize(int width, int height, bool fullscreen) = 0;
    virtual void Shutdown() = 0;
    virtual bool BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Clear(float r, float g, float b, float a) = 0;
    virtual bool IsValid() const = 0;
    virtual void* GetNativeContext() = 0;
    virtual const char* GetName() const = 0;
};

class GraphicsInitializer {
public:
    GraphicsInitializer();
    ~GraphicsInitializer();

    bool Initialize(int width, int height, bool fullscreen, const std::string& windowTitle = "Rayman Legends", const std::string& rendererType = "dx9");
    
    void Shutdown();
    
    std::shared_ptr<IGraphicsRenderer> GetRenderer() const { return renderer_; }
    
    bool IsValid() const;
    
#ifdef _WIN32
    DX9Context* GetDXContext() const;
#endif
    SDL2Context* GetSDLContext() const;

private:
    std::shared_ptr<IGraphicsRenderer> renderer_;
    std::string windowTitle_;
    int width_;
    int height_;
    bool fullscreen_;
    bool initialized_;
}; 