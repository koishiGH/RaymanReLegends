#include "regconfig.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>

static bool ReadRegIntAny(HKEY hKey, const char* name, int& out) {
    DWORD val = 0;
    DWORD size = sizeof(val);
    DWORD type = 0;
    LONG res = RegQueryValueExA(hKey, name, nullptr, &type, (LPBYTE)&val, &size);
    if (res == ERROR_SUCCESS && type == REG_DWORD) {
        out = static_cast<int>(val);
        std::cout << name << ": (REG_DWORD) " << out << std::endl;
        return true;
    }
    char buf[256] = {0};
    size = sizeof(buf);
    res = RegQueryValueExA(hKey, name, nullptr, &type, (LPBYTE)buf, &size);
    if (res == ERROR_SUCCESS && type == REG_SZ) {
        out = std::stoi(buf);
        std::cout << name << ": (REG_SZ) " << out << std::endl;
        return true;
    }
    std::cerr << "Failed to read value: " << name << " (error " << res << ", type " << type << ")" << std::endl;
    return false;
}

static bool ReadRegBoolAny(HKEY hKey, const char* name, bool& out) {
    int val = 0;
    if (ReadRegIntAny(hKey, name, val)) {
        out = (val != 0);
        return true;
    }
    return false;
}

static bool ReadRegFloatDWORD(HKEY hKey, const char* name, float& out) {
    DWORD val = 0;
    DWORD size = sizeof(val);
    DWORD type = 0;
    LONG res = RegQueryValueExA(hKey, name, nullptr, &type, (LPBYTE)&val, &size);
    if (res == ERROR_SUCCESS && type == REG_DWORD) {
        out = *reinterpret_cast<float*>(&val);
        std::cout << name << ": (REG_DWORD) " << out << std::endl;
        return true;
    }
    std::cerr << "Failed to read value: " << name << " (error " << res << ", type " << type << ")" << std::endl;
    return false;
}

bool LoadRegConfig(RegConfig& config) {
    HKEY hKey;
    const char* subkey = "SOFTWARE\\Ubisoft\\Rayman Legends\\Settings";
    LONG openResult = RegOpenKeyExA(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);
    if (openResult != ERROR_SUCCESS) {
        std::cerr << "Could not open registry key: " << subkey << " (error " << openResult << ")" << std::endl;
        return false;
    }
    bool ok = true;
    ok &= ReadRegIntAny(hKey, "ScreenWidth", config.width);
    ok &= ReadRegIntAny(hKey, "ScreenHeight", config.height);
    ok &= ReadRegBoolAny(hKey, "FullScreen", config.fullscreen);
    ok &= ReadRegFloatDWORD(hKey, "MusicVolume", config.musicVolume);
    ok &= ReadRegFloatDWORD(hKey, "SoundVolume", config.soundVolume);
    RegCloseKey(hKey);
    return ok;
}
#else
static std::string GetConfigPath() {
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        return "./rayman_legends.conf";
    }
    
#ifdef __APPLE__
    return std::string(homeDir) + "/Library/Application Support/RaymanLegends/rayman_legends.conf";
#else
    return std::string(homeDir) + "/.config/raymanlegends/rayman_legends.conf";
#endif
}

static bool EnsureConfigDirectory(const std::string& configPath) {
    std::filesystem::path path(configPath);
    std::filesystem::path dir = path.parent_path();
    
    if (!std::filesystem::exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to create config directory: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}

static bool ReadConfigValue(const std::string& line, const std::string& key, int& out) {
    if (line.substr(0, key.length()) == key && line[key.length()] == '=') {
        std::string value = line.substr(key.length() + 1);
        try {
            out = std::stoi(value);
            std::cout << key << ": " << out << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse integer value for " << key << ": " << value << std::endl;
        }
    }
    return false;
}

static bool ReadConfigValue(const std::string& line, const std::string& key, bool& out) {
    if (line.substr(0, key.length()) == key && line[key.length()] == '=') {
        std::string value = line.substr(key.length() + 1);
        if (value == "true" || value == "1") {
            out = true;
            std::cout << key << ": true" << std::endl;
            return true;
        } else if (value == "false" || value == "0") {
            out = false;
            std::cout << key << ": false" << std::endl;
            return true;
        } else {
            std::cerr << "Failed to parse boolean value for " << key << ": " << value << std::endl;
        }
    }
    return false;
}

static bool ReadConfigValue(const std::string& line, const std::string& key, float& out) {
    if (line.substr(0, key.length()) == key && line[key.length()] == '=') {
        std::string value = line.substr(key.length() + 1);
        try {
            out = std::stof(value);
            std::cout << key << ": " << out << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse float value for " << key << ": " << value << std::endl;
        }
    }
    return false;
}

bool LoadRegConfig(RegConfig& config) {
    std::string configPath = GetConfigPath();
    
    if (!std::filesystem::exists(configPath)) {
        if (!EnsureConfigDirectory(configPath)) {
            std::cerr << "Could not create config directory, using defaults" << std::endl;
            return false;
        }
        
        std::ofstream configFile(configPath);
        if (configFile.is_open()) {
            configFile << "# Rayman Legends Configuration File\n";
            configFile << "# Generated automatically - edit at your own risk\n\n";
            configFile << "ScreenWidth=" << config.width << "\n";
            configFile << "ScreenHeight=" << config.height << "\n";
            configFile << "FullScreen=" << (config.fullscreen ? "true" : "false") << "\n";
            configFile << "MusicVolume=" << config.musicVolume << "\n";
            configFile << "SoundVolume=" << config.soundVolume << "\n";
            configFile.close();
            std::cout << "Created default config file: " << configPath << std::endl;
            return true;
        } else {
            std::cerr << "Could not create config file: " << configPath << std::endl;
            return false;
        }
    }
    
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Could not open config file: " << configPath << std::endl;
        return false;
    }
    
    std::string line;
    bool ok = false;
    while (std::getline(configFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (ReadConfigValue(line, "ScreenWidth", config.width) ||
            ReadConfigValue(line, "ScreenHeight", config.height) ||
            ReadConfigValue(line, "FullScreen", config.fullscreen) ||
            ReadConfigValue(line, "MusicVolume", config.musicVolume) ||
            ReadConfigValue(line, "SoundVolume", config.soundVolume)) {
            ok = true;
        }
    }
    
    configFile.close();
    return ok;
}
#endif