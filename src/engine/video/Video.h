#pragma once
#include <string>
#include <vector>

#ifdef _WIN32
#include <bink/bink.h>
#include <windows.h>
#endif

#include <SDL2/SDL.h>

class VideoPlayer {
public:
    VideoPlayer();
    ~VideoPlayer();
    bool Open(const std::string& path, int a3 = 0, int a4 = 0, int a5 = 0);
    void Stop();
    void Start();
    
#ifdef _WIN32
    bool Update(HWND hwnd);
#endif
    bool Update(SDL_Window* window);
    
    bool IsPlaying() const;
    bool HasFinished() const;
private:
#ifdef _WIN32
    HBINK binkHandle = nullptr;
#endif
    bool playing = false;
    bool finished = false;
    int width = 0;
    int height = 0;
    
#ifdef _WIN32
    void* dibBits = nullptr;
    HDC hdc = nullptr;
    HBITMAP hBitmap = nullptr;
    HDC memDC = nullptr;
    HGDIOBJ oldBmp = nullptr;
    bool blitInitialized = false;
    HWND hwndForBlit = nullptr;
#endif

    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    SDL_Texture* videoTexture = nullptr;
    bool sdlInitialized = false;
};
