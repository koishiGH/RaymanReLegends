#pragma once
#include <string>

struct RegConfig {
    int width = 1920;
    int height = 1080;
    bool fullscreen = true;
    float musicVolume = 1.0f; // ignore what I said previously, I'm stupid!
    float soundVolume = 1.0f; // ignore what I said previously, I'm stupid!
};

bool LoadRegConfig(RegConfig& config);
