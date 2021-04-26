#include "AUI/Image.h"
#include "AUI/Core.h"
#include <SDL_Render.h>

namespace AUI {

Image::Image(Screen& screen, const char* key, const SDL_Rect& screenExtent)
: Component(screen, key, screenExtent)
, currentTexHandle()
, currentTexExtent{}
{
}

void Image::addResolution(const ScreenResolution& resolution, const std::string& relPath)
{
    // Attempt to load the given texture (errors on failure).
    ResourceManager& resourceManager = Core::GetResourceManager();
    TextureData textureData;
    textureData.handle = resourceManager.loadTexture(entt::hashed_string(relPath.c_str()));

    // Default the texture extent to the actual texture size.
    SDL_QueryTexture(&(textureData.handle.get()), nullptr, nullptr, &(textureData.extent.w), &(textureData.extent.h));

    // If we already have the given resolution, fail.
    if (resolutionMap.find(resolution) != resolutionMap.end()) {
        AUI_LOG_ERROR("Tried to add image resolution that is already in use. Key: %s, Path: %s", key, relPath);
    }

    // Add the resolution to the map.
    resolutionMap[resolution] = textureData;

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void Image::addResolution(const ScreenResolution& resolution, const std::string& relPath, const SDL_Rect& inTexExtent)
{
    // Do all the same steps from the less specific overload.
    addResolution(resolution, relPath);

    // Set the texture extent to the given extent.
    resolutionMap[resolution].extent = inTexExtent;
}

void Image::render(int offsetX, int offsetY)
{
    if (!currentTexHandle) {
        AUI_LOG_ERROR("Tried to render Image with no texture. Key: %s", key.data());
    }

    // Keep our scaling up to date.
    refreshScaling();

    // Account for the given offset.
    SDL_Rect offsetTex{currentTexExtent};
    offsetTex.x += offsetX;
    offsetTex.y += offsetY;

    SDL_Rect offsetScreen{actualScreenExtent};
    offsetScreen.x += offsetX;
    offsetScreen.y += offsetY;

    // Render the image.
    SDL_RenderCopy(Core::GetRenderer(), &(*currentTexHandle), &offsetTex, &offsetScreen);
}

bool Image::refreshScaling()
{
    // If actualScreenExtent was refreshed, do our specialized refreshing.
    if (Component::refreshScaling()) {
        // Re-calculate which resolution of texture to use.
        refreshChosenResolution();

        return true;
    }

    return false;
}

void Image::refreshChosenResolution()
{
    // If we have a texture that matches the current actualScreenSize.
    auto matchIt = resolutionMap.find(Core::GetActualScreenSize());
    if (matchIt != resolutionMap.end()) {
        // Use the matching texture.
        currentTexHandle = matchIt->second.handle;
        currentTexExtent = matchIt->second.extent;
    }
    else {
        // Else, default to the largest texture for the best chance at nice
        // scaling.
        auto largestIt = resolutionMap.rbegin();
        currentTexHandle = largestIt->second.handle;
        currentTexExtent = largestIt->second.extent;
    }
}

} // namespace AUI
