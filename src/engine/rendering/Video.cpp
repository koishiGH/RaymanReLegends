#include "Video.h"
#include <iostream>
#include <cstring>
#include <bink/bink.h>
#include <windows.h>

VideoPlayer::VideoPlayer() {
    BinkSetSoundSystem(BinkOpenDirectSound, 0);
}

VideoPlayer::~VideoPlayer() {
    Stop();
}

bool VideoPlayer::Open(const std::string& path, int a3, int a4, int a5) {
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
}

void VideoPlayer::Start() {
    playing = true;
    if (binkHandle) BinkGoto(binkHandle, 1, 0);
}

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
            return false;
        }
    } else {
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    }

    return true;
}

bool VideoPlayer::IsPlaying() const { return playing; }

void VideoPlayer::Stop() {
    playing = false;
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
}