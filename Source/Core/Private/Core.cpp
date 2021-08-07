#include "AUI/Core.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

namespace AUI
{
SDL_Renderer* Core::sdlRenderer{nullptr};
ScreenResolution Core::logicalScreenSize{};
ScreenResolution Core::actualScreenSize{};
std::unique_ptr<AssetCache> Core::assetCache{nullptr};
std::atomic<int> Core::componentCount{0};

void Core::initialize(SDL_Renderer* inSdlRenderer,
                      ScreenResolution inLogicalScreenSize)
{
    sdlRenderer = inSdlRenderer;
    assetCache = std::make_unique<AssetCache>();

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

void Core::quit()
{
    // Check if any components are still alive.
    // Components must be destructed before IMG_Quit()/TTF_Quit() or they may
    // segfault when trying to close their resources.
    if (componentCount != 0) {
        AUI_LOG_ERROR("Please destruct all UI components before calling "
                      "AUI::Core::Quit(). Component count: %d",
                      componentCount.load());
    }

    sdlRenderer = nullptr;
    assetCache = nullptr;

    IMG_Quit();
    TTF_Quit();
}

void Core::setActualScreenSize(ScreenResolution inActualScreenSize)
{
    actualScreenSize = inActualScreenSize;
}

SDL_Renderer* Core::getRenderer()
{
    return sdlRenderer;
}

AssetCache& Core::getAssetCache()
{
    return *assetCache;
}

ScreenResolution Core::getLogicalScreenSize()
{
    return logicalScreenSize;
}

ScreenResolution Core::getActualScreenSize()
{
    return actualScreenSize;
}

void Core::incComponentCount()
{
    componentCount++;
}

void Core::decComponentCount()
{
    componentCount--;
}

} // namespace AUI
