#pragma once
#include <string>

struct GameConfig {
    int width = 800;
    int height = 600;
    bool fullscreen = false;
    int waitVBL = 1;
    int disableRendering = 0;
    int logTimings = 0;

    float bgColorR = 0.0f;
    float bgColorG = 0.0f;
    float bgColorB = 0.0f;

    float zoomFactor = 0.2f;
    float depthFactor = 0.1f;
    float iconMinSize = 10.0f;
    int useBigFont = 1;

    int videoCapture = 0;
    int audioCapture = 0;
    int nowarnings = 0;

    int xInputDevicesOnly = 1;
    int disablePadRumble = 0;
};

bool LoadConfig(const std::string& filename, GameConfig& config);
