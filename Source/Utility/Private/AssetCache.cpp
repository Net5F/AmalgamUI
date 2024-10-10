#include "AUI/AssetCache.h"
#include "AUI/Core.h"

namespace AUI
{
std::shared_ptr<SDL_Texture>
    AssetCache::requestTexture(const std::string& textureID)
{
    // If the texture is already in the cache, return it.
    auto it{textureCache.find(textureID)};
    if (it != textureCache.end()) {
        return it->second;
    }

    // The ID wasn't found in the cache, assume it's a path to an image and 
    // try to load it.
    SDL_Texture* rawTexture{
        IMG_LoadTexture(Core::getRenderer(), textureID.c_str())};
    if (rawTexture == nullptr) {
        AUI_LOG_ERROR("Failed to load texture: %s", textureID.c_str());
        return nullptr;
    }

    // Wrap the texture in a shared_ptr.
    std::shared_ptr<SDL_Texture> texture{
        rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); }};

    // Save the texture in the cache.
    textureCache[textureID] = texture;

    return texture;
}

std::shared_ptr<SDL_Texture>
    AssetCache::addTexture(SDL_Texture* inTexture, const std::string& textureID)
{
    // If a texture with the given ID is already in the cache, free it and 
    // replace it with the given texture.
    auto it{textureCache.find(textureID)};
    if (it != textureCache.end()) {
        it->second.reset(inTexture,
                         [](SDL_Texture* p) { SDL_DestroyTexture(p); });
        return it->second;
    }

    // Wrap the texture in a shared_ptr.
    std::shared_ptr<SDL_Texture> texture{
        inTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); }};

    // Save the texture in the cache.
    textureCache[textureID] = texture;

    return texture;
}

std::shared_ptr<TTF_Font> AssetCache::requestFont(const std::string& fontPath,
                                                  int fontSize,
                                                  int fontOutlineSize)
{
    // Prepare the cache ID for this font ("fontPath_fontSize_fontOutlineSize").
    std::string idString{fontPath};
    idString += "_" + std::to_string(fontSize);
    idString += "_" + std::to_string(fontOutlineSize);

    // If the font is already loaded, return it.
    auto it = fontCache.find(idString);
    if (it != fontCache.end()) {
        return it->second;
    }

    // Load the font.
    TTF_Font* rawFont{TTF_OpenFont(fontPath.c_str(), fontSize)};
    if (rawFont == nullptr) {
        AUI_LOG_ERROR("Failed to load font: %s", fontPath.c_str());
        return nullptr;
    }

    // Wrap the font in a shared_ptr.
    std::shared_ptr<TTF_Font> font{std::shared_ptr<TTF_Font>(
        rawFont, [](TTF_Font* p) { TTF_CloseFont(p); })};

    // If non-zero, set the font outline.
    if (fontOutlineSize > 0) {
        TTF_SetFontOutline(font.get(), fontOutlineSize);
    }

    // Save the font in the cache.
    fontCache[idString] = font;

    return font;
}

} // End namespace AUI
