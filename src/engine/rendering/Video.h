#pragma once
#include <string>
#include <vector>
#include <bink/bink.h>
#include <windows.h>

class VideoPlayer {
public:
    VideoPlayer();
    ~VideoPlayer();
    bool Open(const std::string& path, int a3 = 0, int a4 = 0, int a5 = 0);
    void Play(HWND hwnd);
    void Stop();
    void Start();
    bool Update(HWND hwnd);
    bool IsPlaying() const;
private:
    HBINK binkHandle = nullptr;
    bool playing = false;
    void* dibBits = nullptr;
    int width = 0;
    int height = 0;
    HDC hdc = nullptr;
    HBITMAP hBitmap = nullptr;
    HDC memDC = nullptr;
    HGDIOBJ oldBmp = nullptr;
    bool blitInitialized = false;
    HWND hwndForBlit = nullptr;
};
