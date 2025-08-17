#include "GraphicsInitializer.h"
#include <algorithm>
#include <iostream>
#include "DirectXRenderer.h"

GraphicsInitializer::GraphicsInitializer() 
    : width_(0), height_(0), fullscreen_(false), initialized_(false) {
}

GraphicsInitializer::~GraphicsInitializer() {
    Shutdown();
}

bool GraphicsInitializer::Initialize(int width, int height, bool fullscreen, const std::string& windowTitle, const std::string& rendererType) {
    if (initialized_) {
        return true;
    }

    width_ = width;
    height_ = height;
    fullscreen_ = fullscreen;
    windowTitle_ = windowTitle;

    std::string rendererTypeLower = rendererType;
    std::transform(rendererTypeLower.begin(), rendererTypeLower.end(), rendererTypeLower.begin(), ::tolower);

#ifdef _WIN32
    if (rendererTypeLower == "dx9" || rendererTypeLower == "directx" || rendererTypeLower == "auto") {
        renderer_ = std::make_shared<DirectXRenderer>();
    } else {
        std::cerr << "Unknown renderer type: " << rendererType << ". Using DirectX 9." << std::endl;
        renderer_ = std::make_shared<DirectXRenderer>();
    }
#else
    std::cerr << "Error: No renderer available for this platform." << std::endl;
    return false;
#endif

    if (!renderer_) {
        return false;
    }

    if (!renderer_->Initialize(width, height, fullscreen)) {
        renderer_.reset();
        return false;
    }

    initialized_ = true;
    return true;
}

void GraphicsInitializer::Shutdown() {
    if (renderer_) {
        renderer_->Shutdown();
        renderer_.reset();
    }
    initialized_ = false;
}

bool GraphicsInitializer::IsValid() const {
    return initialized_ && renderer_ && renderer_->IsValid();
}

#ifdef _WIN32
DX9Context* GraphicsInitializer::GetDXContext() const {
    if (renderer_ && renderer_->IsValid()) {
        return static_cast<DX9Context*>(renderer_->GetNativeContext());
    }
    return nullptr;
}
#endif