#pragma once

#include "AUI/Internal/ResourceManager.h"
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
     */
    static void Initialize(const std::string& inResourcePath, SDL_Renderer* inSdlRenderer);

    /**
     * Cleans up this library and SDL_image/SDL_ttf.
     * Don't call unless you're ready to also tear down SDL_image and SDL_ttf.
     *
     * Errors if componentCount != 0.
     */
    static void Quit();

    static const std::string& GetResourcePath();
    static SDL_Renderer* GetRenderer();
    static AUI::ResourceManager& GetResourceManager();
    static void IncComponentCount();
    static void DecComponentCount();

private:
    /** The base path to the start of the resource directory. */
    static std::string resourcePath;

    /** The renderer to use when constructing textures and rendering. */
    static SDL_Renderer* sdlRenderer;

    /** The resource manager for font objects and image textures. */
    static std::unique_ptr<ResourceManager> resourceManager;

    /** Keeps a count of the number of currently constructed components.
        Used to check if it's safe to Quit(). */
    static std::atomic<int> componentCount;
};

} // namespace AUI
