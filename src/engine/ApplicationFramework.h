#pragma once

#include <string>
#include <memory>
#include "platform/window.h"
#include "system/regconfig.h"
#include "system/config.h"
#include "graphics/GraphicsInitializer.h"
#include "video/Video.h"
#include "resource/Bundle.h"

class ApplicationFramework {
public:
    ApplicationFramework();
    ~ApplicationFramework();

    bool init(int argc, char* argv[]);    
    int update();    
    void display(float deltaTime);
    void destroy();    
    bool isVideoPlaying() const;

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
