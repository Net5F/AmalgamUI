#include "AUI/ResourceManager.h"
#include "AUI/Core.h"

namespace AUI
{
TextureHandle ResourceManager::loadTexture(const std::string& relPath)
{
    // Prepare the cache ID for this texture (we just use the relPath).
    entt::hashed_string id{relPath.c_str()};

    // Append Core::resourcePath to the given path.
    std::string fullPath{Core::GetResourcePath()};
    fullPath += relPath;

    // Load the texture.
    // Note: If the texture is already loaded, this returns the existing
    //       handle.
    TextureHandle handle = textureCache.load<TextureLoader>(id, fullPath, Core::GetRenderer());
    if (!handle) {
        AUI_LOG_ERROR("Failed to load texture at path: %s", fullPath.c_str());
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

    // Prepare the path.
    std::string fullPath{Core::GetResourcePath()};
    fullPath += relPath.data();

    // Load the font.
    FontHandle handle = fontCache.load<FontLoader>(id, fullPath, size);
    if (!handle) {
        AUI_LOG_ERROR("Failed to load font: %s", fullPath.c_str());
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
