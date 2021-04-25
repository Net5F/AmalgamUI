#include "AUI/Core.h"
#include <SDL_Render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace AUI {

std::string Core::resourcePath{""};
SDL_Renderer* Core::sdlRenderer{nullptr};
ScreenResolution Core::logicalScreenSize{};
ScreenResolution Core::actualScreenSize{};
std::unique_ptr<ResourceManager> Core::resourceManager{nullptr};
std::atomic<int> Core::componentCount{0};

void Core::Initialize(const std::string& inResourcePath, SDL_Renderer* inSdlRenderer
                      , ScreenResolution inLogicalScreenSize)
{
    resourcePath = inResourcePath;
    sdlRenderer = inSdlRenderer;
    resourceManager = std::make_unique<ResourceManager>();

    // Set the screen sizes. Default actual to the same as logical.
    logicalScreenSize = inLogicalScreenSize;
    actualScreenSize = inLogicalScreenSize;

    // Initialize SDL_img (safe to call if already initialized).
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    // Initialize SDL_ttf if it hasn't already been called.
    if (TTF_WasInit() == 0) {
        TTF_Init();
    }
}

void Core::Quit()
{
    // Check if any components are still alive.
    // Components must be destructed before IMG_Quit()/TTF_Quit() or they may
    // segfault when trying to close their resources.
    if (componentCount != 0) {
        AUI_LOG_ERROR("Please destruct all UI components before calling "
        "AUI::Core::Quit(). Component count: %d", componentCount.load());
    }

    resourcePath = "";
    sdlRenderer = nullptr;
    resourceManager = nullptr;

    IMG_Quit();
    TTF_Quit();
}

void Core::setActualScreenSize(ScreenResolution inActualScreenSize)
{
    actualScreenSize = inActualScreenSize;
}

void Core::IncComponentCount()
{
    componentCount++;
}

void Core::DecComponentCount()
{
    componentCount--;
}

const std::string& Core::GetResourcePath()
{
    return resourcePath;
}

SDL_Renderer* Core::GetRenderer()
{
    return sdlRenderer;
}

ResourceManager& Core::GetResourceManager()
{
    return *resourceManager;
}

ScreenResolution Core::GetLogicalScreenSize()
{
    return logicalScreenSize;
}

ScreenResolution Core::GetActualScreenSize()
{
    return actualScreenSize;
}

} // namespace AUI
