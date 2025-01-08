#pragma once

#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace AUI
{

/**
 * This class facilitates loading and managing the lifetime of assets.
 *
 * TODO: We need to find an appropriate time to prune the cache.
 *       If we made our own managed pointer, we could do it when references are
 *       dropped, or we could track how long an asset has gone without having
 *       any references and periodically remove old ones.
 */
class AssetCache
{
public:
    // Note: We use const std::string& instead of std::string_view because we
    //       need to pass C strings into the SDL APIs.
    /**
     * If a texture with the given ID is in the cache, returns it.
     * If not and the ID is a valid file path to an image, adds the image to 
     * the cache and returns it.
     *
     * @param textureID A user-defined ID (for textures added using 
     *                  addTexture()), or the full path to an image file.
     * @param scaleMode The filtering/scaling mode that this texture should use.
     * @return A valid texture if one was found, else nullptr.
     */
    std::shared_ptr<SDL_Texture> requestTexture(const std::string& textureID,
                                                SDL_ScaleMode scaleMode);

    /**
     * Adds the given texture to the cache, using the given ID.
     * If a texture already exists with the given ID, it will be overwritten.
     *
     * Note: Ownership of the texture will be taken. Do not free it.
     *
     * @return A valid texture.
     */
    std::shared_ptr<SDL_Texture> addTexture(SDL_Texture* inTexture,
                                            const std::string& textureID);

    /**
     * If a font associated with the given path and size is in the cache,
     * returns it.
     * If not, loads it and adds it to the cache, then returns it.
     *
     * @param fontPath The full path to the font file.
     * @param fontSize The size of the font, in points.
     * @param fontOutlineSize The size of the font's outline.
     */
    std::shared_ptr<TTF_Font> requestFont(const std::string& fontPath,
                                          int fontSize, int fontOutlineSize);

private:
    std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureCache;

    std::unordered_map<std::string, std::shared_ptr<TTF_Font>> fontCache;
};

} // End namespace AUI
