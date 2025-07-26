#include <iostream>
#include "engine/core/window.h"
#include "engine/core/regconfig.h"
#include "engine/core/config.h"
#include "engine/core/DXInit.h"
#include "engine/rendering/Video.h"
#include "engine/resource/Bundle.h"

int main() {
    GameConfig config;
    LoadConfig("config.xml", config);
    RegConfig regConfig;
    LoadRegConfig(regConfig);

    HWND hwnd = CreateMainWindow(regConfig.width, regConfig.height, regConfig.fullscreen);
    if (!hwnd) {
        std::cerr << "Failed to create window!" << std::endl;
        return 1;
    }

    DX9Context dx;
    if (!InitDirect3D9(dx, hwnd, regConfig.width, regConfig.height, regConfig.fullscreen)) {
        std::cerr << "Failed to initialize Direct3D 9!" << std::endl;
        return 1;
    }
    else {
        std::cout << "Direct3D 9 initialized successfully!" << std::endl;
    }

    VideoPlayer video;
    if (!video.Open("video/ubilogo_lums_framework_720p.bik")) {
        std::cerr << "Failed to open video: video/ubilogo_lums_framework_720p.bik" << std::endl;
    } else {
        std::cout << "Video opened successfully!" << std::endl;
    }
    video.Start();

    try {
        Bundle bundle;
        Bundle fullLogic;
        Bundle persistentLoading;
        
        if (!bundle.openBundle("Bundle_PC.ipk")) {
            std::cerr << "Failed to open Bundle_PC.ipk" << std::endl;
        } else {
            std::cout << "Bundle_PC.ipk opened successfully!" << std::endl;
            auto files = bundle.listFiles();
            std::cout << "Bundle contains " << files.size() << " files" << std::endl;
        }
        
        if (!fullLogic.openBundle("fullLogic_PC.ipk")) {
            std::cerr << "Failed to open fullLogic_PC.ipk" << std::endl;
        } else {
            std::cout << "fullLogic_PC.ipk opened successfully!" << std::endl;
        }
        
        if (!persistentLoading.openBundle("persistentLoading_PC.ipk")) {
            std::cerr << "Failed to open persistentLoading_PC.ipk" << std::endl;
        } else {
            std::cout << "persistentLoading_PC.ipk opened successfully!" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load bundles: " << e.what() << std::endl;
    }
    
    bool running = true;
    while (running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (video.IsPlaying()) {
            video.Update(hwnd);
        }
        Sleep(1);
    }

    if (dx.sprite) dx.sprite->Release();
    if (dx.deviceEx) dx.deviceEx->Release();
    if (dx.device && !dx.deviceEx) dx.device->Release();
    if (dx.d3d9ex) dx.d3d9ex->Release();
    if (dx.d3d9 && !dx.d3d9ex) dx.d3d9->Release();
    if (dx.d3d9Dll) FreeLibrary(dx.d3d9Dll);

    return 0;
}