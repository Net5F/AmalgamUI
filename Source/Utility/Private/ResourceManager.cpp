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

} // End namespace AUI
