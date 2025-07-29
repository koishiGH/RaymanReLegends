#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif
#include "engine/platform/window.h"
#include "engine/system/regconfig.h"
#include "engine/system/config.h"
#include "engine/graphics/GraphicsInitializer.h"
#include "engine/graphics/SDL2Renderer.h"
#include "engine/video/Video.h"
#include "engine/resource/Bundle.h"
#include <SDL2/SDL.h>

std::string ParseRendererType(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.substr(0, 10) == "-renderer=") {
            return arg.substr(10);
        }
    }
    return "auto";
}

int main(int argc, char* argv[]) {
    GameConfig config;
    LoadConfig("config.xml", config);
    RegConfig regConfig;
    LoadRegConfig(regConfig);

    std::string rendererType = ParseRendererType(argc, argv);
    std::cout << "Requested renderer: " << rendererType << std::endl;
    
    GraphicsInitializer graphics;
    
    if (!graphics.Initialize(regConfig.width, regConfig.height, regConfig.fullscreen, "Rayman Legends", rendererType)) {
        std::cerr << "Failed to initialize graphics system!" << std::endl;
        return 1;
    }

    std::cout << "Graphics system initialized successfully! Using: " << graphics.GetRenderer()->GetName() << std::endl;

    auto renderer = graphics.GetRenderer();
    if (!renderer) {
        std::cerr << "Failed to get graphics renderer!" << std::endl;
        return 1;
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
        
        /*
        // I honestly don't think this is needed due to it not being in the steam version and only in the UbiConnect version.
        if (!fullLogic.openBundle("fullLogic_PC.ipk")) {
            std::cerr << "Failed to open fullLogic_PC.ipk" << std::endl;
        } else {
            std::cout << "fullLogic_PC.ipk opened successfully!" << std::endl;
        }
        */
        
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
        if (strcmp(renderer->GetName(), "SDL2") == 0) {
            auto sdlRenderer = static_cast<SDL2Renderer*>(renderer.get());
            if (sdlRenderer && sdlRenderer->ShouldQuit()) {
                running = false;
                break;
            }
        }
        
#ifdef _WIN32
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (video.IsPlaying()) {
            if (strcmp(renderer->GetName(), "SDL2") == 0) {
                auto sdlRenderer = static_cast<SDL2Renderer*>(renderer.get());
                if (sdlRenderer) {
                    sdlRenderer->HandleEvents();
                }
            }
            
            auto rendererName = renderer->GetName();
            if (strcmp(rendererName, "DirectX 9") == 0) {
                auto dxContext = graphics.GetDXContext();
                if (dxContext && dxContext->hwnd) {
                    video.Update(dxContext->hwnd);
                }
            } else if (strcmp(rendererName, "SDL2") == 0) {
                auto sdlContext = graphics.GetSDLContext();
                if (sdlContext && sdlContext->window) {
                    video.Update(static_cast<SDL_Window*>(sdlContext->window));
                }
            }
            Sleep(16);
        } else {
            if (!renderer->BeginFrame()) {
                break;
            }
            renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);
            renderer->EndFrame();
        }
#else
        if (video.IsPlaying()) {
            if (strcmp(renderer->GetName(), "SDL2") == 0) {
                auto sdlRenderer = static_cast<SDL2Renderer*>(renderer.get());
                if (sdlRenderer) {
                    sdlRenderer->HandleEvents();
                }
            }
            
            auto rendererName = renderer->GetName();
            if (strcmp(rendererName, "SDL2") == 0) {
                auto sdlContext = graphics.GetSDLContext();
                if (sdlContext && sdlContext->window) {
                    video.Update(static_cast<SDL_Window*>(sdlContext->window));
                }
            }
            SDL_Delay(16);
        } else {
            if (!renderer->BeginFrame()) {
                break;
            }
            renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);
            renderer->EndFrame();
        }
#endif
#ifdef _WIN32
        Sleep(1);
#else
        SDL_Delay(1);
#endif
    }

    graphics.Shutdown();
    std::cout << "Graphics system shut down successfully!" << std::endl;
    return 0;
}