#include "AUI/AssetCache.h"
#include "AUI/Core.h"

namespace AUI
{
std::shared_ptr<SDL_Texture>
    AssetCache::requestTexture(const std::string& imagePath)
{
    // If the texture is already loaded, return it.
    auto it{textureCache.find(imagePath)};
    if (it != textureCache.end()) {
        return it->second;
    }

    // Load the texture.
    SDL_Texture* rawTexture{
        IMG_LoadTexture(Core::getRenderer(), imagePath.c_str())};
    if (rawTexture == nullptr) {
        AUI_LOG_ERROR("Failed to load texture: %s", imagePath.c_str());
    }

    // Wrap the texture in a shared_ptr.
    std::shared_ptr<SDL_Texture> texture{
        rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); }};

    // Save the texture in the cache.
    textureCache[imagePath] = texture;

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
