#pragma once
#include <string>

struct RegConfig {
    int width = 1920;
    int height = 1080;
    bool fullscreen = true;
    float musicVolume = 1.0f; // you can't change this in-game idk why lol!
    float soundVolume = 1.0f; // you can't change this in-game idk why lol!
};

bool LoadRegConfig(RegConfig& config);
