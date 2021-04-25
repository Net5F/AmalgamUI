#pragma once

#include "AUI/Internal/ResourceManager.h"
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
     * @param inResourcePath  The base path to use when opening resource files.
     *                        Provided for convenience, can be whatever is
     *                        useful for you.
     * @param inSdlRenderer  The renderer to use for constructing textures and
     *                       rendering.
     * @param inLogicalScreenSize  See setActualScreenSize().
     */
    static void Initialize(const std::string& inResourcePath, SDL_Renderer* inSdlRenderer
                           , ScreenResolution inLogicalScreenSize);

    /**
     * Cleans up this library and SDL_image/SDL_ttf.
     * Don't call unless you're ready to also tear down SDL_image and SDL_ttf.
     *
     * Errors if componentCount != 0.
     */
    static void Quit();

    /**
     * Sets a new actual screen size for the UI to scale to.
     *
     * Components will recognize that this size changed, prompting them to re-
     * calculate their relevant data.
     *
     * UI scaling works through two concepts: logical screen size, and actual.
     * The intent is that the developer will set a logical size and give all
     * width, height, position, etc values in reference to that logical size.
     * Then, the user can select a new actual size and the UI will scale to it
     * intelligently.
     */
    static void setActualScreenSize(ScreenResolution inActualScreenSize);

    static const std::string& GetResourcePath();
    static SDL_Renderer* GetRenderer();
    static AUI::ResourceManager& GetResourceManager();
    static ScreenResolution GetLogicalScreenSize();
    static ScreenResolution GetActualScreenSize();

private:
    /** Friend component so it can update the component count. */
    friend class Component;

    /**
     * Increases the count of currently constructed components.
     */
    static void IncComponentCount();
    /**
     * Decreases the count of currently constructed components.
     */
    static void DecComponentCount();

    /** The base path to the start of the resource directory. */
    static std::string resourcePath;

    /** The renderer to use when constructing textures and rendering. */
    static SDL_Renderer* sdlRenderer;

    /** See setActualScreenSize(). */
    static ScreenResolution logicalScreenSize;

    /** See setActualScreenSize(). */
    static ScreenResolution actualScreenSize;

    /** The resource manager for font objects and image textures. */
    static std::unique_ptr<ResourceManager> resourceManager;

    /** Keeps a count of the number of currently constructed components.
        Used to check if it's safe to Quit(). */
    static std::atomic<int> componentCount;
};

} // namespace AUI
