#include "config.h"
#include "tinyxml2.h"
#include <iostream>
#include <string>
#include <cstring>

using namespace tinyxml2;

static int GetIntAttr(XMLElement* elem, const char* attr, int def) {
    int val = def;
    if (elem) elem->QueryIntAttribute(attr, &val);
    return val;
}

static float GetFloatAttr(XMLElement* elem, const char* attr, float def) {
    float val = def;
    if (elem) elem->QueryFloatAttribute(attr, &val);
    return val;
}

bool LoadConfig(const std::string& filename, GameConfig& config) {
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load config: " << filename << std::endl;
        return false;
    }

    XMLElement* root = doc.FirstChildElement("root");
    if (!root) {
        std::cerr << "No <root> element in config.xml" << std::endl;
        return false;
    }

    if (auto screen = root->FirstChildElement("Screen")) {
        config.width = GetIntAttr(screen, "Width", config.width);
        config.height = GetIntAttr(screen, "Height", config.height);
        config.fullscreen = GetIntAttr(screen, "FullScreen", config.fullscreen) != 0;
    }

    if (auto bg = root->FirstChildElement("BGColor")) {
         config.bgColorR = GetFloatAttr(bg, "R", config.bgColorR);
         config.bgColorG = GetFloatAttr(bg, "G", config.bgColorG);
         config.bgColorB = GetFloatAttr(bg, "B", config.bgColorB);
    }

    if (auto editor = root->FirstChildElement("Editor")) {
        config.zoomFactor = GetFloatAttr(editor, "ZoomFactor", config.zoomFactor);
        config.depthFactor = GetFloatAttr(editor, "DepthFactor", config.depthFactor);
        config.iconMinSize = GetFloatAttr(editor, "IconMinSize", config.iconMinSize);
    }

    if (auto debug = root->FirstChildElement("Debug")) {
        config.videoCapture = GetIntAttr(debug, "VideoCapture", config.videoCapture);
        config.audioCapture = GetIntAttr(debug, "AudioCapture", config.audioCapture);
    }

    if (auto pc = root->FirstChildElement("PC")) {
        config.xInputDevicesOnly = GetIntAttr(pc, "XInputDevicesOnly", config.xInputDevicesOnly);
    }
    return true;
}