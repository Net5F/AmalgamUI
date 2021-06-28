#pragma once

#include "AUI/Internal/Log.h"

#include <SDL_Render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "entt/core/hashed_string.hpp"
#include "entt/resource/cache.hpp"
#include "entt/resource/loader.hpp"

#include <memory>
#include <string>

namespace AUI
{

using TextureHandle = entt::resource_handle<SDL_Texture>;
using FontHandle = entt::resource_handle<TTF_Font>;

/**
 * This class facilitates loading and managing the lifetime of texture and
 * resources.
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
     * @param relPath  The path to the texture, including the file name,
     *                 relative to Core's resourcePath.
     */
    TextureHandle loadTexture(const std::string& relPath);

    /**
     * Removes the texture at the given path from the resource cache.
     *
     * @param relPath  The path to the texture, including the file name,
     *                 relative to Core's resourcePath.
     * @return true if the texture was found and removed, else false.
     */
    bool discardTexture(const std::string& relPath);

    /**
     * Loads the font at the given path and size, and returns a handle to it.
     *
     * If the font is already loaded, returns a handle to it without re-
     * loading.
     *
     * @param relPath  The path to the font, including the file name, relative
     *                 to Core's resourcePath.
     * @param size  The size of the font, in points.
     */
    FontHandle loadFont(const std::string& relPath, int size);

    /**
     * Removes the font at the given path from the resource cache.
     *
     * @param relPath  The path to the font, including the file name, relative
     *                 to Core's resourcePath.
     * @param size  The size of the font, in points.
     * @return true if the font was found and removed, else false.
     */
    bool discardFont(const std::string& relPath, int size);

private:
    entt::resource_cache<SDL_Texture> textureCache;

    entt::resource_cache<TTF_Font> fontCache;
};

/**
 * Specialized loader for SDL_Texture.
 */
struct TextureLoader : entt::resource_loader<TextureLoader, SDL_Texture> {
    std::shared_ptr<SDL_Texture> load(const std::string& filePath,
                                          SDL_Renderer* sdlRenderer) const
    {
        SDL_Texture* texture = IMG_LoadTexture(sdlRenderer, filePath.c_str());
        if (texture == nullptr) {
            AUI_LOG_ERROR("Failed to load texture: %s", filePath.c_str());
        }

        return std::shared_ptr<SDL_Texture>(texture, [](SDL_Texture* p){SDL_DestroyTexture(p);});
    }
};

/**
 * Specialized loader for TTF_Font.
 */
struct FontLoader : entt::resource_loader<FontLoader, TTF_Font> {
    std::shared_ptr<TTF_Font> load(const std::string& fontPath, int fontSize) const
    {
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (font == nullptr) {
            AUI_LOG_ERROR("Failed to load font: %s", fontPath.c_str());
        }

        return std::shared_ptr<TTF_Font>(font, [](TTF_Font* p){TTF_CloseFont(p);});
    }
};

} // End namespace AUI
