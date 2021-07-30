#pragma once

#include "AUI/Internal/Log.h"

#include <SDL_Render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace AUI
{

using TextureHandle = std::shared_ptr<SDL_Texture>;
using FontHandle = std::shared_ptr<TTF_Font>;

/**
 * This class facilitates loading and managing the lifetime of font resources.
 */
class ResourceManager
{
public:
    /**
     * Loads the texture at the given path and returns a handle to it.
     *
     * If the texture is already loaded, returns a handle to it without re-
     * loading.
     *
     * @param filePath  The path to the texture, including the file name,
     *                  relative to Core's resourcePath.
     */
    TextureHandle loadTexture(const std::string& filePath);

    /**
     * Removes the texture at the given path from the resource cache.
     *
     * @param filePath  The full path to the texture, including the file name.
     * @return true if the texture was found and removed, else false.
     */
    bool discardTexture(const std::string& filePath);

    /**
     * Loads the font at the given path and size, and returns a handle to it.
     *
     * If the font is already loaded, returns a handle to it without re-
     * loading.
     *
     * @param filePath  The full path to the font, including the file name.
     * @param size  The size of the font, in points.
     */
    FontHandle loadFont(const std::string& filePath, int size);

    /**
     * Removes the font at the given path from the resource cache.
     *
     * @param filePath  The path to the font, including the file name, relative
     *                 to Core's resourcePath.
     * @param size  The size of the font, in points.
     * @return true if the font was found and removed, else false.
     */
    bool discardFont(const std::string& filePath, int size);

private:
    std::unordered_map<std::string, TextureHandle> textureCache;

    std::unordered_map<std::string, FontHandle> fontCache;
};

} // End namespace AUI
