#include "AUI/ImageType/NineSliceImage.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"
#include "AUI/Internal/AUIAssert.h"

namespace AUI
{
void NineSliceImage::set(const std::string& imagePath, SliceSizes inSliceSizes,
                         const SDL_Rect& renderExtent)
{
    // Attempt to load the image (errors on failure).
    AssetCache& assetCache{Core::getAssetCache()};
    sourceTexture = assetCache.requestTexture(imagePath);

    // Save the new slice sizes (will be used by the regenerate function).
    sliceSizes = inSliceSizes;

    // We're going to generate a texture as large as the given extent.
    currentTexExtent.w = renderExtent.w;
    currentTexExtent.h = renderExtent.h;

    // Re-generate our nine slice texture.
    regenerateNineSliceTexture();
}

void NineSliceImage::refresh(const SDL_Rect& renderExtent)
{
    // Set the new desired extent.
    currentTexExtent.w = renderExtent.w;
    currentTexExtent.h = renderExtent.h;

    // Re-generate our nine slice texture.
    regenerateNineSliceTexture();
}

void NineSliceImage::regenerateNineSliceTexture()
{
    // Get the texture's pixel format and size.
    Uint32 pixelFormat{};
    int sourceWidth{};
    int sourceHeight{};
    SDL_QueryTexture(sourceTexture.get(), &pixelFormat, nullptr, &sourceWidth,
                     &sourceHeight);

    // Check that we have enough room to get our slices.
    AUI_ASSERT(sourceWidth > (sliceSizes.left + sliceSizes.right + 1),
               "Source texture too narrow for given slice sizes.");
    AUI_ASSERT(sourceHeight > (sliceSizes.top + sliceSizes.bottom + 1),
               "Source texture too short for given slice sizes.");

    // Allocate the new texture.
    SDL_Texture* rawTexture{SDL_CreateTexture(
        Core::getRenderer(), pixelFormat, SDL_TEXTUREACCESS_TARGET,
        currentTexExtent.w, currentTexExtent.h)};
    if (rawTexture == nullptr) {
        AUI_LOG_FATAL("Failed to create texture: %s", SDL_GetError());
    }
    auto nineSliceTexture{std::shared_ptr<SDL_Texture>(
        rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); })};

    // Set the new texture as the render target and copy the slices.
    SDL_SetRenderTarget(Core::getRenderer(), nineSliceTexture.get());
    copyCorners(sourceWidth, sourceHeight);
    copySides(sourceWidth, sourceHeight);
    copyCenter(sourceWidth, sourceHeight);

    // Set the render target back to the window.
    SDL_SetRenderTarget(Core::getRenderer(), nullptr);

    // Set the generated texture as our new current texture.
    currentTexture = nineSliceTexture;
}

void NineSliceImage::copyCorners(int sourceWidth, int sourceHeight)
{
    /* Copy the corners, with no scaling. */
    // Top left
    SDL_Rect sourceRect = {0, 0, sliceSizes.left, sliceSizes.top};
    SDL_Rect destRect = {0, 0, sliceSizes.left, sliceSizes.top};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);

    // Top right
    sourceRect = {(sourceWidth - sliceSizes.right), 0, sliceSizes.right,
                  sliceSizes.top};
    destRect = {(currentTexExtent.w - sliceSizes.right), 0, sliceSizes.right,
                sliceSizes.top};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);

    // Bottom left
    sourceRect = {0, (sourceHeight - sliceSizes.bottom), sliceSizes.left,
                  sliceSizes.bottom};
    destRect = {0, (currentTexExtent.h - sliceSizes.bottom), sliceSizes.left,
                sliceSizes.bottom};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);

    // Bottom right
    sourceRect
        = {(sourceWidth - sliceSizes.right), (sourceHeight - sliceSizes.bottom),
           sliceSizes.right, sliceSizes.bottom};
    destRect = {(currentTexExtent.w - sliceSizes.right),
                (currentTexExtent.h - sliceSizes.bottom), sliceSizes.right,
                sliceSizes.bottom};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);
}

void NineSliceImage::copySides(int sourceWidth, int sourceHeight)
{
    /* Copy the sides, stretching them in one direction to fit. */
    // Top
    SDL_Rect sourceRect
        = {sliceSizes.left, 0,
           (sourceWidth - sliceSizes.left - sliceSizes.right), sliceSizes.top};
    SDL_Rect destRect
        = {sliceSizes.left, 0,
           (currentTexExtent.w - sliceSizes.left - sliceSizes.right),
           sliceSizes.top};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);

    // Bottom
    sourceRect = {sliceSizes.left, (sourceHeight - sliceSizes.bottom),
                  (sourceWidth - sliceSizes.left - sliceSizes.right),
                  sliceSizes.bottom};
    destRect = {sliceSizes.left, (currentTexExtent.h - sliceSizes.bottom),
                (currentTexExtent.w - sliceSizes.left - sliceSizes.right),
                sliceSizes.bottom};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);

    // Left
    sourceRect = {0, sliceSizes.top, sliceSizes.left,
                  (sourceHeight - sliceSizes.top - sliceSizes.bottom)};
    destRect = {0, sliceSizes.top, sliceSizes.left,
                (currentTexExtent.h - sliceSizes.top - sliceSizes.bottom)};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);

    // Right
    sourceRect
        = {(sourceWidth - sliceSizes.right), sliceSizes.top, sliceSizes.right,
           (sourceHeight - sliceSizes.top - sliceSizes.bottom)};
    destRect = {(currentTexExtent.w - sliceSizes.right), sliceSizes.top,
                sliceSizes.right,
                (currentTexExtent.h - sliceSizes.top - sliceSizes.bottom)};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);
}

void NineSliceImage::copyCenter(int sourceWidth, int sourceHeight)
{
    // Copy the center, stretching it in both directions to fit.
    SDL_Rect sourceRect = {sliceSizes.left, sliceSizes.top,
                           (sourceWidth - sliceSizes.left - sliceSizes.right),
                           (sourceHeight - sliceSizes.top - sliceSizes.bottom)};
    SDL_Rect destRect
        = {sliceSizes.left, sliceSizes.top,
           (currentTexExtent.w - sliceSizes.left - sliceSizes.right),
           (currentTexExtent.h - sliceSizes.top - sliceSizes.bottom)};
    SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(), &sourceRect,
                   &destRect);
}

} // namespace AUI
