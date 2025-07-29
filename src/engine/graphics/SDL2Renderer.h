#pragma once

#include "GraphicsInitializer.h"
#include <SDL2/SDL.h>

class SDL2Renderer : public IGraphicsRenderer {
public:
    SDL2Renderer();
    ~SDL2Renderer();

    bool Initialize(int width, int height, bool fullscreen) override;
    void Shutdown() override;
    bool BeginFrame() override;
    void EndFrame() override;
    void Clear(float r, float g, float b, float a) override;
    bool IsValid() const override;
    void* GetNativeContext() override;
    const char* GetName() const override;

    bool CreateSDLWindow(const std::string& title);
    bool CreateRenderer();
    void HandleEvents();
    bool ShouldQuit() const;

private:
    SDL2Context context_;
    bool valid_;
    bool shouldQuit_;
}; 