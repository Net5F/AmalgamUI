#include "AUI/Image.h"
#include "AUI/Core.h"
#include <SDL_Render.h>

namespace AUI {

Image::Image(Screen& inScreen, const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Component(inScreen, inLogicalExtent, inDebugName)
, currentTexHandle()
, currentTexExtent{}
{
}

void Image::addResolution(const ScreenResolution& resolution, const std::string& relPath)
{
    // Attempt to load the given texture (errors on failure).
    ResourceManager& resourceManager = Core::getResourceManager();
    TextureData textureData;
    textureData.handle = resourceManager.loadTexture(relPath);

    // Default the texture extent to the actual texture size.
    SDL_QueryTexture(textureData.handle.get(), nullptr, nullptr, &(textureData.extent.w), &(textureData.extent.h));

    // If we already have the given resolution, fail.
    if (resolutionMap.find(resolution) != resolutionMap.end()) {
        AUI_LOG_ERROR("Tried to add image resolution that is already in use. "
        "DebugName: %s, Resolution: (%d, %d)", debugName.c_str(), resolution.width, resolution.height);
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

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void Image::render(const SDL_Point& parentOffset)
{
    // Keep our scaling up to date.
    refreshScaling();

    // Account for the given offset.
    SDL_Rect offsetExtent{scaledExtent};
    offsetExtent.x += parentOffset.x;
    offsetExtent.y += parentOffset.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = offsetExtent;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // If we don't have a texture to render, fail.
    if (!currentTexHandle) {
        AUI_LOG_ERROR("Tried to render Image with no texture. DebugName: %s", debugName.c_str());
    }

    // Render the image.
    SDL_RenderCopy(Core::getRenderer(), &(*currentTexHandle)
        , &currentTexExtent, &lastRenderedExtent);
}

void Image::clearTextures()
{
    currentTexHandle = TextureHandle();
    currentTexExtent = SDL_Rect{};
    resolutionMap.clear();
}

bool Image::refreshScaling()
{
    // If scaledExtent was refreshed, do our specialized refreshing.
    if (Component::refreshScaling()) {
        // Re-calculate which resolution of texture to use.
        refreshChosenResolution();

        return true;
    }

    return false;
}

void Image::refreshChosenResolution()
{
    if (resolutionMap.size() == 0) {
        // No resolutions to choose from, return early.
        return;
    }

    // If we have a texture that matches the current actualScreenSize.
    auto matchIt = resolutionMap.find(Core::getActualScreenSize());
    if (matchIt != resolutionMap.end()) {
        // Use the matching texture.
        currentTexHandle = matchIt->second.handle;
        currentTexExtent = matchIt->second.extent;
    }
    else {
        // Else, default to the largest texture for the best chance at nice
        // scaling.
        // Note: This relies on resolutionMap being sorted, hence why we use
        //       std::map.
        auto largestIt = resolutionMap.rbegin();
        currentTexHandle = largestIt->second.handle;
        currentTexExtent = largestIt->second.extent;
    }
}

} // namespace AUI
