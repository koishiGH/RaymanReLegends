#include "Video.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <bink/bink.h>
#include <windows.h>
#endif
#include <SDL2/SDL.h>

VideoPlayer::VideoPlayer() {
#ifdef _WIN32
    BinkSetSoundSystem(BinkOpenDirectSound, 0);
#endif
}

VideoPlayer::~VideoPlayer() {
    Stop();
}

bool VideoPlayer::Open(const std::string& path, int a3, int a4, int a5) {
#ifdef _WIN32
    binkHandle = BinkOpen(path.c_str(), BINKSNDTRACK);
    if (!binkHandle) {
        std::cerr << "Failed to open Bink video: " << path << std::endl;
        return false;
    }

    BINKFRAMEBUFFERS fbInfo;
    BinkGetFrameBuffersInfo(binkHandle, &fbInfo);

    BINKSUMMARY summary;
    BinkGetSummary(binkHandle, &summary);
    width = summary.Width;
    height = summary.Height;

    std::cout << "Bink video opened: " << path << std::endl;
    std::cout << "Width: " << summary.Width << ", Height: " << summary.Height << std::endl;
    std::cout << "Frames: " << summary.TotalFrames << ", FrameRate: " << summary.FrameRate << std::endl;

    return true;
#else
    std::cerr << "Bink video playback not supported on this platform" << std::endl;
    return false;
#endif
}

void VideoPlayer::Start() {
    playing = true;
    finished = false;
#ifdef _WIN32
    if (binkHandle) BinkGoto(binkHandle, 1, 0);
#endif
}

#ifdef _WIN32
bool VideoPlayer::Update(HWND hwnd) {
    if (!playing || !binkHandle) return false;

    if (!blitInitialized) {
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        hdc = GetDC(hwnd);
        hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &dibBits, nullptr, 0);
        memDC = CreateCompatibleDC(hdc);
        oldBmp = SelectObject(memDC, hBitmap);
        hwndForBlit = hwnd;
        blitInitialized = true;
    }

    if (!BinkWait(binkHandle)) {
        BinkDoFrame(binkHandle);
        BinkCopyToBuffer(
            binkHandle,
            dibBits,
            width * 4,
            height,
            0, 0,
            BINKSURFACE32);
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
        BinkNextFrame(binkHandle);

        if (binkHandle->FrameNum >= binkHandle->Frames) {
            playing = false;
            finished = true;
            return false;
        }
    } else {
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    }

    return true;
}
#endif

bool VideoPlayer::Update(SDL_Window* window) {
#ifdef _WIN32
    if (!playing || !binkHandle || !window) return false;
#else
    if (!playing || !window) return false;
    std::cerr << "Video playback not supported on this platform....yet!" << std::endl;
    playing = false;
    finished = true;
    return false;
#endif

#ifdef _WIN32
    if (!sdlInitialized) {
        sdlWindow = window;
        sdlRenderer = SDL_GetRenderer(window);
        if (!sdlRenderer) {
            std::cerr << "Failed to get SDL renderer for video playback" << std::endl;
            return false;
        }
        
        videoTexture = SDL_CreateTexture(
            sdlRenderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width, height
        );
        
        if (!videoTexture) {
            std::cerr << "Failed to create SDL texture for video: " << SDL_GetError() << std::endl;
            return false;
        }
        
        sdlInitialized = true;
    }

    if (!BinkWait(binkHandle)) {
        BinkDoFrame(binkHandle);
        
        void* pixels;
        int pitch;
        if (SDL_LockTexture(videoTexture, nullptr, &pixels, &pitch) == 0) {
            BinkCopyToBuffer(
                binkHandle,
                pixels,
                pitch,
                height,
                0, 0,
                BINKSURFACE32
            );
            SDL_UnlockTexture(videoTexture);
            SDL_RenderClear(sdlRenderer);
            SDL_RenderCopy(sdlRenderer, videoTexture, nullptr, nullptr);
            SDL_RenderPresent(sdlRenderer);
        }
        
        BinkNextFrame(binkHandle);

        if (binkHandle->FrameNum >= binkHandle->Frames) {
            playing = false;
            finished = true;
            return false;
        }
    }

    return true;
#endif
}

bool VideoPlayer::IsPlaying() const { return playing; }

bool VideoPlayer::HasFinished() const { return finished; }

void VideoPlayer::Stop() {
    playing = false;
    finished = false;
#ifdef _WIN32
    if (binkHandle) {
        BinkClose(binkHandle);
        binkHandle = nullptr;
    }
    
    if (blitInitialized) {
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        if (hwndForBlit && hdc) ReleaseDC(hwndForBlit, hdc);
        blitInitialized = false;
        dibBits = nullptr;
        hBitmap = nullptr;
        memDC = nullptr;
        oldBmp = nullptr;
        hdc = nullptr;
        hwndForBlit = nullptr;
    }
#endif
    
    if (sdlInitialized) {
        if (videoTexture) {
            SDL_DestroyTexture(videoTexture);
            videoTexture = nullptr;
        }
        sdlRenderer = nullptr;
        sdlWindow = nullptr;
        sdlInitialized = false;
    }
}