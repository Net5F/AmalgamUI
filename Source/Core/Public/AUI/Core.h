#pragma once

#include "AUI/ResourceManager.h"
#include "AUI/ScreenResolution.h"
#include <string>
#include <memory>
#include <atomic>

// Forward declarations.
struct SDL_Renderer;

namespace AUI {

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
     * Call this before constructing any components.
     *
     * Alternatively, use the Initializer class.
     *
     * @param inSdlRenderer  The renderer to use for constructing textures and
     *                       rendering.
     * @param inLogicalScreenSize  See ScalingHelpers.h class comment.
     */
    static void initialize(SDL_Renderer* inSdlRenderer
                           , ScreenResolution inLogicalScreenSize);

    /**
     * Cleans up this library and SDL_image/SDL_ttf.
     * Don't call unless you're ready to also tear down SDL_image and SDL_ttf.
     *
     * Errors if componentCount != 0.
     */
    static void quit();

    /**
     * Sets a new screen size for the UI to scale to.
     *
     * Components will recognize that this size changed, prompting them to re-
     * calculate their relevant data.
     *
     * See ScalingHelpers.h class comment for full information on AUI scaling.
     */
    static void setActualScreenSize(ScreenResolution inScaledScreenSize);

    static SDL_Renderer* getRenderer();
    static AUI::ResourceManager& getResourceManager();
    static ScreenResolution getLogicalScreenSize();
    static ScreenResolution getActualScreenSize();

private:
    /** Friend component so it can update the component count. */
    friend class Component;

    /**
     * Increases the count of currently constructed components.
     */
    static void incComponentCount();
    /**
     * Decreases the count of currently constructed components.
     */
    static void decComponentCount();

    /** The renderer to use when constructing textures and rendering. */
    static SDL_Renderer* sdlRenderer;

    /** See ScalingHelpers.h class comment. */
    static ScreenResolution logicalScreenSize;

    /** See ScalingHelpers.h class comment. */
    static ScreenResolution actualScreenSize;

    /** The resource manager for font objects and image textures. */
    static std::unique_ptr<ResourceManager> resourceManager;

    /** Keeps a count of the number of currently constructed components.
        Used to check if it's safe to Quit(). */
    static std::atomic<int> componentCount;
};

} // namespace AUI
