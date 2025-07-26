#include "regconfig.h"
#include <windows.h>
#include <string>
#include <iostream>

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