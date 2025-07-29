#include "SDL2Renderer.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cstring>

SDL2Renderer::SDL2Renderer() : valid_(false), shouldQuit_(false) {
    memset(&context_, 0, sizeof(context_));
}

SDL2Renderer::~SDL2Renderer() {
    Shutdown();
}

bool SDL2Renderer::Initialize(int width, int height, bool fullscreen) {
    if (valid_) {
        return true;
    }

    context_.width = width;
    context_.height = height;
    context_.fullscreen = fullscreen;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL2 initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!CreateSDLWindow("Rayman Legends")) {
        std::cerr << "Failed to create SDL2 window!" << std::endl;
        return false;
    }

    if (!CreateRenderer()) {
        std::cerr << "Failed to create SDL2 renderer!" << std::endl;
        return false;
    }

    valid_ = true;
    return true;
}

void SDL2Renderer::Shutdown() {
    if (context_.texture) {
        SDL_DestroyTexture(static_cast<SDL_Texture*>(context_.texture));
        context_.texture = nullptr;
    }
    if (context_.renderer) {
        SDL_DestroyRenderer(static_cast<SDL_Renderer*>(context_.renderer));
        context_.renderer = nullptr;
    }
    if (context_.window) {
        SDL_DestroyWindow(static_cast<SDL_Window*>(context_.window));
        context_.window = nullptr;
    }
    SDL_Quit();
    valid_ = false;
}

bool SDL2Renderer::BeginFrame() {
    if (!valid_ || !context_.renderer) {
        return false;
    }

    HandleEvents();
    return true;
}

void SDL2Renderer::EndFrame() {
    if (!valid_ || !context_.renderer) {
        return;
    }

    SDL_RenderPresent(static_cast<SDL_Renderer*>(context_.renderer));
}

void SDL2Renderer::Clear(float r, float g, float b, float a) {
    if (!valid_ || !context_.renderer) {
        return;
    }

    SDL_SetRenderDrawColor(static_cast<SDL_Renderer*>(context_.renderer),
        static_cast<Uint8>(r * 255.0f),
        static_cast<Uint8>(g * 255.0f),
        static_cast<Uint8>(b * 255.0f),
        static_cast<Uint8>(a * 255.0f));
    SDL_RenderClear(static_cast<SDL_Renderer*>(context_.renderer));
}

bool SDL2Renderer::IsValid() const {
    return valid_;
}

void* SDL2Renderer::GetNativeContext() {
    return &context_;
}

const char* SDL2Renderer::GetName() const {
    return "SDL2";
}

bool SDL2Renderer::CreateSDLWindow(const std::string& title) {
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (context_.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    SDL_Window* window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        context_.width, context_.height,
        flags
    );

    if (!window) {
        std::cerr << "Failed to create SDL2 window: " << SDL_GetError() << std::endl;
        return false;
    }

    context_.window = window;
    return true;
}

bool SDL2Renderer::CreateRenderer() {
    if (!context_.window) {
        return false;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        static_cast<SDL_Window*>(context_.window),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "Failed to create SDL2 renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    context_.renderer = renderer;
    return true;
}

void SDL2Renderer::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            shouldQuit_ = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                shouldQuit_ = true;
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                context_.width = event.window.data1;
                context_.height = event.window.data2;
            }
            break;
        }
    }
}

bool SDL2Renderer::ShouldQuit() const {
    return shouldQuit_;
} 