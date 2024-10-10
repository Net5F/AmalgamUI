#include "AUI/ImageType/SimpleImage.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"

namespace AUI
{
void SimpleImage::set(const std::string& textureID)
{
    // Attempt to load the image.
    if (currentTexture = Core::getAssetCache().requestTexture(textureID)) {
        // Set the texture extent to the actual texture size.
        SDL_QueryTexture(currentTexture.get(), nullptr, nullptr,
                         &(currentTexExtent.w), &(currentTexExtent.h));
    }
}

void SimpleImage::set(const std::string& textureID,
                      const SDL_Rect& inTexExtent)
{
    // Attempt to load the image.
    if (currentTexture = Core::getAssetCache().requestTexture(textureID)) {
        // Set the texture extent to the given extent.
        currentTexExtent = inTexExtent;
    }
}

void SimpleImage::clear()
{
    currentTexture = nullptr;
    currentTexExtent = SDL_Rect{};
}

} // namespace AUI
