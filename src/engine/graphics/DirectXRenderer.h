#pragma once

#include "GraphicsInitializer.h"

#ifdef _WIN32

class DirectXRenderer : public IGraphicsRenderer {
public:
    DirectXRenderer();
    ~DirectXRenderer();

    bool Initialize(int width, int height, bool fullscreen) override;
    void Shutdown() override;
    bool BeginFrame() override;
    void EndFrame() override;
    void Clear(float r, float g, float b, float a) override;
    bool IsValid() const override;
    void* GetNativeContext() override;
    const char* GetName() const override;

    bool CreateDevice(HWND hwnd);
    bool CreateSprite();
    void ReleaseResources();

private:
    DX9Context context_;
    bool valid_;
};

#endif 