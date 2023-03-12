#include "AUI/Image.h"
#include "AUI/Core.h"

namespace AUI
{
Image::Image(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, currentTexture{nullptr}
, currentTexExtent{}
{
}

void Image::addResolution(const ScreenResolution& resolution,
                          const std::string& relPath)
{
    // If we already have the given resolution, fail.
    if (resolutionMap.find(resolution) != resolutionMap.end()) {
        AUI_LOG_FATAL("Tried to add image resolution that is already in use. "
                      "DebugName: %s, Resolution: (%d, %d)",
                      debugName.c_str(), resolution.width, resolution.height);
    }

    // Start constructing the TextureData.
    TextureData textureData;
    textureData.relPath = relPath;
    textureData.userProvidedExtent = false;

    // Add the resolution to the map.
    resolutionMap[resolution] = textureData;

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void Image::addResolution(const ScreenResolution& resolution,
                          const std::string& relPath,
                          const SDL_Rect& inTexExtent)
{
    // Do all the same steps from the less specific overload.
    addResolution(resolution, relPath);

    // Set the texture extent to the given extent.
    resolutionMap[resolution].extent = inTexExtent;
    resolutionMap[resolution].userProvidedExtent = true;

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void Image::render()
{
    // If we don't have a texture to render, fail.
    if (currentTexture == nullptr) {
        AUI_LOG_FATAL("Tried to render Image with no texture. DebugName: %s",
                      debugName.c_str());
    }

    // Render the image.
    SDL_RenderCopy(Core::getRenderer(), currentTexture.get(), &currentTexExtent,
                   &renderExtent);
}

void Image::clearTextures()
{
    currentTexture = nullptr;
    currentTexExtent = SDL_Rect{};
    resolutionMap.clear();
}

bool Image::refreshScaling()
{
    // If scaledExtent was refreshed, do our specialized refreshing.
    if (Widget::refreshScaling()) {
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
    TextureData* selectedTextureData{nullptr};
    auto matchIt = resolutionMap.find(Core::getActualScreenSize());
    if (matchIt != resolutionMap.end()) {
        // Select the matching texture data.
        selectedTextureData = &(matchIt->second);
    }
    else {
        // Else, default to the largest texture for the best chance at nice
        // scaling.
        // Note: This relies on resolutionMap being sorted, hence why we use
        //       std::map.
        auto largestIt = resolutionMap.rbegin();
        selectedTextureData = &(largestIt->second);
    }

    // Attempt to load the matching image (errors on failure).
    AssetCache& assetCache{Core::getAssetCache()};
    currentTexture = assetCache.requestTexture(selectedTextureData->relPath);

    // If the user provided an extent, use it.
    if (selectedTextureData->userProvidedExtent) {
        currentTexExtent = selectedTextureData->extent;
    }
    else {
        // No user-provided extent, use the actual texture size.
        SDL_QueryTexture(currentTexture.get(), nullptr, nullptr,
                         &(currentTexExtent.w), &(currentTexExtent.h));
    }
}

} // namespace AUI
