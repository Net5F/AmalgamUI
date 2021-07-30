#include "AUI/ResourceManager.h"
#include "AUI/Core.h"

namespace AUI
{
TextureHandle ResourceManager::loadTexture(const std::string& filePath)
{
    // If the texture is already loaded, return it.
    auto it = textureCache.find(filePath);
    if (it != textureCache.end()) {
        return it->second;
    }

    // Load the texture.
    SDL_Texture* texture = IMG_LoadTexture(Core::getRenderer(), filePath.c_str());
    if (texture == nullptr) {
        AUI_LOG_ERROR("Failed to load texture: %s", filePath.c_str());
    }

    // Wrap the texture in a shared_ptr.
    TextureHandle handle = TextureHandle(texture, [](SDL_Texture* p){SDL_DestroyTexture(p);});

    // Save the texture in the cache.
    textureCache[filePath] = handle;

    return handle;
}

bool ResourceManager::discardTexture(const std::string& filePath)
{
    // If the cache contains the given texture, discard it.
    auto it = textureCache.find(filePath);
    if (it != textureCache.end()) {
        textureCache.erase(it);
        return true;
    }
    else {
        return false;
    }
}

FontHandle ResourceManager::loadFont(const std::string& filePath, int size)
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
    TTF_Font* font = TTF_OpenFont(filePath.c_str(), size);
    if (font == nullptr) {
        AUI_LOG_ERROR("Failed to load font: %s", filePath.c_str());
    }

    // Wrap the font in a shared_ptr.
    FontHandle handle = FontHandle(font, [](TTF_Font* p){TTF_CloseFont(p);});

    // Save the font in the cache.
    fontCache[idString] = handle;

    return handle;
}

bool ResourceManager::discardFont(const std::string& filePath, int size)
{
    // Prepare the cache ID for this font ("filePath_size").
    std::string idString{filePath};
    idString += "_" + std::to_string(size);

    // If the cache contains the given font, discard it.
    auto it = fontCache.find(idString);
    if (it != fontCache.end()) {
        fontCache.erase(it);
        return true;
    }
    else {
        return false;
    }
}

} // End namespace AUI
