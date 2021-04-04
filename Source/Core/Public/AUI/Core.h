#pragma once

#include "AUI/Internal/ResourceManager.h"
#include <string>

// Forward declarations.
struct SDL_Renderer;

namespace AUI {

/**
 * This class fulfills two responsibilities:
 *   1. Allows the consumer to configure the library.
 *   2. Maintains common data that library objects need.
 */
class Core
{
public:
    /**
     * Initializes the library. Call this before constructing any objects.
     *
     * @param inResourcePath  The base path to use when opening resource files.
     *                        Provided for convenience, can be whatever is
     *                        useful for you.
     * @param inSdlRenderer  The renderer to use for constructing textures and
     *                       rendering.
     */
    static void Initialize(const std::string& inResourcePath, SDL_Renderer* inSdlRenderer);

    static const std::string& GetResourcePath();
    static SDL_Renderer* GetRenderer();
    static AUI::ResourceManager& GetResourceManager();

private:
    /** The base path to the start of the resource directory. */
    static std::string resourcePath;

    /** The renderer to use when constructing textures and rendering. */
    static SDL_Renderer* sdlRenderer;

    static ResourceManager resourceManager;
};

} // namespace AUI
