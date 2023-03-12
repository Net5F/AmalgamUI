#include "AUI/AssetCache.h"
#include "AUI/Core.h"

namespace AUI
{
std::shared_ptr<SDL_Texture> AssetCache::requestTexture(const std::string& filePath)
{
    // If the texture is already loaded, return it.
    auto it{textureCache.find(filePath)};
    if (it != textureCache.end()) {
        return it->second;
    }

    // Load the texture.
    SDL_Texture* rawTexture{
        IMG_LoadTexture(Core::getRenderer(), filePath.c_str())};
    if (rawTexture == nullptr) {
        AUI_LOG_FATAL("Failed to load texture: %s", filePath.c_str());
    }

    // Wrap the texture in a shared_ptr.
    std::shared_ptr<SDL_Texture> texture{
        std::shared_ptr<SDL_Texture>(rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); })};

    // Save the texture in the cache.
    textureCache[filePath] = texture;

    return texture;
}

std::shared_ptr<TTF_Font> AssetCache::requestFont(const std::string& filePath, int size)
{
    // Prepare the cache ID for this font ("filePath_size").
    std::string idString{filePath};
    idString += "_" + std::to_string(size);

    // If the font is already loaded, return it.
    auto it = fontCache.find(idString);
    if (it != fontCache.end()) {
        return it->second;
    }

    // Load the font.
    TTF_Font* rawFont = TTF_OpenFont(filePath.c_str(), size);
    if (rawFont == nullptr) {
        AUI_LOG_FATAL("Failed to load font: %s", filePath.c_str());
    }

    // Wrap the font in a shared_ptr.
    std::shared_ptr<TTF_Font> font = std::shared_ptr<TTF_Font>(
        rawFont, [](TTF_Font* p) { TTF_CloseFont(p); });

    // Save the font in the cache.
    fontCache[idString] = font;

    return font;
}

} // End namespace AUI
