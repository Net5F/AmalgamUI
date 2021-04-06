#include "AUI/Internal/ResourceManager.h"
#include "AUI/Core.h"

namespace AUI
{
TextureHandle ResourceManager::loadTexture(const entt::hashed_string& relPath)
{
    // If the texture is already loaded, don't try to load it again.
    if (textureCache.contains(relPath)) {
        return getTexture(relPath);
    }

    // Prepare the path.
    std::string fullPath{Core::GetResourcePath()};
    fullPath += relPath.data();

    // Load the texture.
    TextureHandle handle = textureCache.load<TextureLoader>(relPath, fullPath, Core::GetRenderer());
    if (!handle) {
        AUI_LOG_ERROR("Failed to load texture at path: %s", fullPath.c_str());
    }

    return handle;
}

TextureHandle ResourceManager::getTexture(const entt::hashed_string id)
{
    return textureCache.handle(id);
}

FontHandle ResourceManager::loadFont(const std::string& relPath, int size)
{
    // Prepare the ID ("relPath_size").
    std::string idString{relPath};
    idString += "_" + std::to_string(size);
    entt::hashed_string id(idString.c_str());

    // If the font is already loaded, don't try to load it again.
    if (fontCache.contains(id)) {
        return getFont(id);
    }

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

FontHandle ResourceManager::getFont(const entt::hashed_string id)
{
    return fontCache.handle(id);
}

} // End namespace AUI
