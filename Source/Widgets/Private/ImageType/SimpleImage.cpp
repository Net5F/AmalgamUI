#include "AUI/ImageType/SimpleImage.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"

namespace AUI
{
void SimpleImage::set(const std::string& textureID, SDL_ScaleMode scaleMode)
{
    // Attempt to load the image.
    if ((currentTexture
         = Core::getAssetCache().requestTexture(textureID, scaleMode))) {
        // Set the texture extent to the actual texture size.
        SDL_GetTextureSize(currentTexture.get(), &(currentTexExtent.w),
                           &(currentTexExtent.h));
    }
}

void SimpleImage::set(const std::string& textureID,
                      const SDL_FRect& inTexExtent, SDL_ScaleMode scaleMode)
{
    // Attempt to load the image.
    if ((currentTexture
         = Core::getAssetCache().requestTexture(textureID, scaleMode))) {
        // Set the texture extent to the given extent.
        currentTexExtent = inTexExtent;
    }
}

void SimpleImage::clear()
{
    currentTexture = nullptr;
    currentTexExtent = SDL_FRect{};
}

} // namespace AUI
