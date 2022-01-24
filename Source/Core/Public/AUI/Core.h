#pragma once

#include "AUI/AssetCache.h"
#include "AUI/ScreenResolution.h"
#include <string>
#include <memory>
#include <atomic>

// Forward declarations.
struct SDL_Renderer;

namespace AUI
{
/**
 * Fulfills two responsibilities:
 *   1. Allows the consumer to configure the library.
 *   2. Maintains common data that library objects need.
 */
class Core
{
public:
    /**
     * Initializes this library (and SDL_image/SDL_ttf if they haven't been).
     * Call this before constructing any widgets.
     *
     * Alternatively, use the Initializer class.
     *
     * @param inSdlRenderer  The renderer to use for constructing textures and
     *                       rendering.
     * @param inLogicalScreenSize  See ScalingHelpers.h class comment.
     */
    static void initialize(SDL_Renderer* inSdlRenderer,
                           ScreenResolution inLogicalScreenSize);

    /**
     * Cleans up this library and SDL_image/SDL_ttf.
     * Don't call unless you're ready to also tear down SDL_image and SDL_ttf.
     *
     * Errors if widgetCount != 0.
     */
    static void quit();

    /**
     * Sets a new screen size for the UI to scale to.
     *
     * Widgets will recognize that this size changed, prompting them to re-
     * calculate their relevant data.
     *
     * See ScalingHelpers.h class comment for full information on AUI scaling.
     */
    static void setActualScreenSize(ScreenResolution inScaledScreenSize);

    static SDL_Renderer* getRenderer();
    static AUI::AssetCache& getAssetCache();
    static ScreenResolution getLogicalScreenSize();
    static ScreenResolution getActualScreenSize();

private:
    /** Friend widget so it can update the widget count. */
    friend class Widget;

    /**
     * Increases the count of currently constructed widgets.
     */
    static void incWidgetCount();
    /**
     * Decreases the count of currently constructed widgets.
     */
    static void decWidgetCount();

    /** The renderer to use when constructing textures and rendering. */
    static SDL_Renderer* sdlRenderer;

    /** See ScalingHelpers.h class comment. */
    static ScreenResolution logicalScreenSize;

    /** See ScalingHelpers.h class comment. */
    static ScreenResolution actualScreenSize;

    /** The asset cache for font objects. */
    static std::unique_ptr<AssetCache> assetCache;

    /** Keeps a count of the number of currently constructed widgets.
        Used to check if it's safe to Quit(). */
    static std::atomic<int> widgetCount;
};

} // namespace AUI
