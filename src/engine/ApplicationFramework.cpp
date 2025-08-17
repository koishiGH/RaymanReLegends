#include "ApplicationFramework.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif

ApplicationFramework::ApplicationFramework()
    : m_running(false)
    , m_graphicsInitialized(false)
    , m_videoInitialized(false)
    , m_bundlesInitialized(false)
{
}

ApplicationFramework::~ApplicationFramework() {
    destroy();
}

bool ApplicationFramework::init(int argc, char* argv[]) {
    std::cout << "Initializing Application Framework..." << std::endl;
    
    LoadConfig("config.xml", m_config);
    LoadRegConfig(m_regConfig);
    
    m_rendererType = parseRendererType(argc, argv);
    
    if (!initGraphics()) {
        std::cerr << "Failed to initialize graphics system!" << std::endl;
        return false;
    }
    
    if (!initVideo()) {
        std::cerr << "Failed to initialize video system!" << std::endl;
    }
    
    if (!initBundles()) {
        std::cerr << "Failed to initialize resource bundles!" << std::endl;
    }
    
    m_running = true;
    std::cout << "Application Framework initialized successfully!" << std::endl;
    return true;
}

int ApplicationFramework::update() {
    if (!m_running) {
        std::cerr << "Application Framework not initialized!" << std::endl;
        return 1;
    }
    
    if (!processMessages()) {
        return 0;
    }
    
    if (m_videoInitialized && m_video->IsPlaying()) {
        updateVideo();
    } else if (m_graphicsInitialized) {
        updateFrame();
    }
    
    return 1;
}

void ApplicationFramework::display(float deltaTime) {
    if (!isVideoPlaying() && m_graphicsInitialized) {
        displayFrame();
    }
}

bool ApplicationFramework::isVideoPlaying() const {
    return m_videoInitialized && m_video && m_video->IsPlaying();
}

void ApplicationFramework::destroy() {
    std::cout << "Destroying Application Framework..." << std::endl;
    
    if (m_graphicsInitialized) {
        m_graphics->Shutdown();
        m_graphicsInitialized = false;
    }
    m_running = false;
}

std::string ApplicationFramework::parseRendererType(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.substr(0, 10) == "-renderer=") {
            return arg.substr(10);
        }
    }
    return "auto";
}

bool ApplicationFramework::initGraphics() {
    m_graphics = std::make_unique<GraphicsInitializer>();
    
    if (!m_graphics->Initialize(m_regConfig.width, m_regConfig.height, m_regConfig.fullscreen, "Rayman Legends", m_rendererType)) {
        return false;
    }
    
    std::cout << "Graphics system initialized successfully! Using: " << m_graphics->GetRenderer()->GetName() << std::endl;
    
    if (!m_graphics->GetRenderer()) {
        std::cerr << "Failed to get graphics renderer!" << std::endl;
        return false;
    }
    
    m_graphicsInitialized = true;
    return true;
}

bool ApplicationFramework::initVideo() {
    m_video = std::make_unique<VideoPlayer>();
    
    if (!m_video->Open("video/ubilogo_lums_framework_720p.bik")) {
        std::cerr << "Failed to open video: video/ubilogo_lums_framework_720p.bik" << std::endl;
        return false;
    }
    
    m_video->Start();
    m_videoInitialized = true;
    return true;
}

bool ApplicationFramework::initBundles() {
    try {
        m_mainBundle = std::make_unique<Bundle>();
        m_persistentLoadingBundle = std::make_unique<Bundle>();
        if (!m_mainBundle->openBundle("Bundle_PC.ipk")) {
            std::cerr << "Failed to open Bundle_PC.ipk" << std::endl;
            return false;
        }
        std::cout << "Bundle_PC.ipk opened successfully!" << std::endl;
        auto files = m_mainBundle->listFiles();        
        if (!m_persistentLoadingBundle->openBundle("persistentLoading_PC.ipk")) {
            std::cerr << "Failed to open persistentLoading_PC.ipk" << std::endl;
            return false;
        }
        std::cout << "persistentLoading_PC.ipk opened successfully!" << std::endl;
        m_bundlesInitialized = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load bundles: " << e.what() << std::endl;
        return false;
    }
}

bool ApplicationFramework::processMessages() {
#ifdef _WIN32
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            m_running = false;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
    return true;
}

void ApplicationFramework::updateVideo() {
#ifdef _WIN32
    if (m_graphicsInitialized && m_videoInitialized) {
        auto renderer = m_graphics->GetRenderer();
        auto rendererName = renderer->GetName();
        
        if (strcmp(rendererName, "DirectX 9") == 0) {
            auto dxContext = m_graphics->GetDXContext();
            if (dxContext && dxContext->hwnd) {
                m_video->Update(dxContext->hwnd);
            }
        }
    }
#endif
}

void ApplicationFramework::updateFrame() {
    // game stuff here.
}

void ApplicationFramework::displayFrame() {
    if (!m_graphicsInitialized) {
        return;
    }
    
    auto renderer = m_graphics->GetRenderer();
    if (!renderer->BeginFrame()) {
        m_running = false;
        return;
    }
    
    renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);
    renderer->EndFrame();
}
