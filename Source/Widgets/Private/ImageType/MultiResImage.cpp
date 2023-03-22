#include "AUI/ImageType/MultiResImage.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"
#include "AUI/Internal/AUIAssert.h"
#include "AUI/Internal/Ignore.h"

namespace AUI
{
void MultiResImage::addResolution(const ScreenResolution& resolution,
                                  const std::string& imagePath)
{
    // If we already have the given resolution, fail.
    if (resolutionMap.find(resolution) != resolutionMap.end()) {
        AUI_LOG_FATAL("Tried to add image resolution that is already in use. "
                      "Resolution: (%d, %d)",
                      resolution.width, resolution.height);
    }

    // Start constructing the TextureData.
    TextureData textureData;
    textureData.imagePath = imagePath;
    textureData.userProvidedExtent = false;

    // Add the resolution to the map.
    resolutionMap[resolution] = textureData;

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void MultiResImage::addResolution(const ScreenResolution& resolution,
                                  const std::string& imagePath,
                                  const SDL_Rect& texExtent)
{
    // Do all the same steps from the less specific overload.
    addResolution(resolution, imagePath);

    // Set the texture extent to the given extent.
    resolutionMap[resolution].extent = texExtent;
    resolutionMap[resolution].userProvidedExtent = true;

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void MultiResImage::clear()
{
    currentTexture = nullptr;
    currentTexExtent = SDL_Rect{};
    resolutionMap.clear();
}

void MultiResImage::refresh(const SDL_Rect& scaledExtent)
{
    ignore(scaledExtent);

    // Re-calculate which resolution of texture to use.
    refreshChosenResolution();
}

void MultiResImage::refreshChosenResolution()
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
    currentTexture = assetCache.requestTexture(selectedTextureData->imagePath);

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
