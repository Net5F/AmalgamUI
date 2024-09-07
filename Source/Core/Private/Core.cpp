#include "AUI/Core.h"
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace AUI
{
SDL_Renderer* Core::sdlRenderer{nullptr};
ScreenResolution Core::logicalScreenSize{};
ScreenResolution Core::actualScreenSize{};
std::unique_ptr<AssetCache> Core::assetCache{nullptr};
int Core::dragTriggerDistance{10};
int Core::squaredDragTriggerDistance{dragTriggerDistance * dragTriggerDistance};
std::atomic<bool> Core::isTextInputFocused{false};
std::atomic<int> Core::widgetCount{0};

void Core::initialize(SDL_Renderer* inSdlRenderer,
                      ScreenResolution inLogicalScreenSize,
                      ScreenResolution inActualScreenSize)
{
    sdlRenderer = inSdlRenderer;
    assetCache = std::make_unique<AssetCache>();

    // Set the screen sizes.
    logicalScreenSize = inLogicalScreenSize;
    actualScreenSize = inActualScreenSize;

    // Initialize SDL_img (safe to call if already initialized).
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    // Initialize SDL_ttf if it hasn't already been called.
    if (TTF_WasInit() == 0) {
        TTF_Init();
    }
}

void Core::quit()
{
    // Check if any widgets are still alive.
    // Widgets must be destructed before IMG_Quit()/TTF_Quit() or they may
    // segfault when trying to close their resources.
    if (widgetCount != 0) {
        AUI_LOG_FATAL("Please destruct all UI widgets before calling "
                      "AUI::Core::Quit(). Widget count: %d",
                      widgetCount.load());
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

void Core::setDragTriggerDistance(int newDragTriggerDistance)
{
    dragTriggerDistance = newDragTriggerDistance;
    squaredDragTriggerDistance = dragTriggerDistance * dragTriggerDistance;
}

bool Core::getIsTextInputFocused()
{
    return isTextInputFocused;
}

SDL_Renderer* Core::getRenderer()
{
    return sdlRenderer;
}

ScreenResolution Core::getLogicalScreenSize()
{
    return logicalScreenSize;
}

ScreenResolution Core::getActualScreenSize()
{
    return actualScreenSize;
}

AssetCache& Core::getAssetCache()
{
    return *assetCache;
}

int Core::getSquaredDragTriggerDistance()
{
    return squaredDragTriggerDistance;
}

void Core::incWidgetCount()
{
    widgetCount++;
}

void Core::decWidgetCount()
{
    widgetCount--;
}

} // namespace AUI
