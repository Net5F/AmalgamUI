#include "AUI/ResourceManager.h"
#include "AUI/Core.h"

namespace AUI
{
TextureHandle ResourceManager::loadTexture(const std::string& relPath)
{
    // Prepare the cache ID for this texture (we just use the relPath).
    entt::hashed_string id{relPath.c_str()};

    // Load the texture.
    // Note: If the texture is already loaded, this returns the existing
    //       handle.
    TextureHandle handle = textureCache.load<TextureLoader>(id, relPath, Core::getRenderer());
    if (!handle) {
        AUI_LOG_ERROR("Failed to load texture at path: %s", relPath.c_str());
    }

    return handle;
}

bool ResourceManager::discardTexture(const std::string& relPath)
{
    // Prepare the cache ID for this texture (we just use the relPath).
    entt::hashed_string id{relPath.c_str()};

    // If the cache contains the given texture, discard it.
    if (textureCache.contains(id)) {
        textureCache.discard(id);
        return true;
    }
    else {
        return false;
    }
}

FontHandle ResourceManager::loadFont(const std::string& relPath, int size)
{
    // Prepare the cache ID for this font ("relPath_size").
    std::string idString{relPath};
    idString += "_" + std::to_string(size);
    entt::hashed_string id(idString.c_str());

    // Load the font.
    FontHandle handle = fontCache.load<FontLoader>(id, relPath, size);
    if (!handle) {
        AUI_LOG_ERROR("Failed to load font: %s", relPath.c_str());
    }

    return handle;
}

bool ResourceManager::discardFont(const std::string& relPath, int size)
{
    // Prepare the cache ID for this font ("relPath_size").
    std::string idString{relPath};
    idString += "_" + std::to_string(size);
    entt::hashed_string id(idString.c_str());

    // If the cache contains the given font, discard it.
    if (textureCache.contains(id)) {
        textureCache.discard(id);
        return true;
    }
    else {
        return false;
    }
}

} // End namespace AUI
