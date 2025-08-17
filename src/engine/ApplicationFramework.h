#pragma once

#include <string>
#include <memory>
#include "engine/platform/window.h"
#include "engine/system/regconfig.h"
#include "engine/system/config.h"
#include "engine/graphics/GraphicsInitializer.h"
#include "engine/video/Video.h"
#include "engine/resource/Bundle.h"

class ApplicationFramework {
public:
    ApplicationFramework();
    ~ApplicationFramework();

    bool init(int argc, char* argv[]);    
    int update();    
    void display(float deltaTime);
    void destroy();

private:
    std::string parseRendererType(int argc, char* argv[]);
    bool initGraphics();
    bool initVideo();
    bool initBundles();
    
    bool processMessages();
    void updateVideo();
    void updateFrame();
    
    void displayFrame();

private:
    GameConfig m_config;
    RegConfig m_regConfig;
    std::string m_rendererType;
    
    std::unique_ptr<GraphicsInitializer> m_graphics;
    std::unique_ptr<VideoPlayer> m_video;
    
    std::unique_ptr<Bundle> m_mainBundle;
    std::unique_ptr<Bundle> m_persistentLoadingBundle;
    
    bool m_running;
    bool m_graphicsInitialized;
    bool m_videoInitialized;
    bool m_bundlesInitialized;
};
