#include "AUI/ImageType/TiledImage.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"
#include "AUI/Internal/Log.h"
#include <SDL3/SDL_render.h>

namespace AUI
{
void TiledImage::set(const std::string& textureID,
                     const SDL_FRect& scaledExtent)
{
    // Attempt to load the image.
    // Note: We assume that tiled textures will want to use "nearest" scaling
    //       to maintain sharpness.
    if ((sourceTexture = Core::getAssetCache().requestTexture(
             textureID, SDL_SCALEMODE_NEAREST))) {
        // We're going to generate a texture as large as the given extent.
        currentTexExtent.w = scaledExtent.w;
        currentTexExtent.h = scaledExtent.h;

        // Re-generate our tiled texture.
        regenerateTiledTexture();
    }
}

void TiledImage::refresh(const SDL_FRect& scaledExtent)
{
    // Set the new desired extent.
    currentTexExtent.w = scaledExtent.w;
    currentTexExtent.h = scaledExtent.h;

    // Re-generate our tiled texture.
    regenerateTiledTexture();
}

void TiledImage::regenerateTiledTexture()
{
    // Get the texture's pixel format and size.
    SDL_PixelFormat pixelFormat{sourceTexture->format};
    float sourceWidth{};
    float sourceHeight{};
    SDL_GetTextureSize(sourceTexture.get(), &sourceWidth, &sourceHeight);

    // Allocate the new texture.
    SDL_Texture* rawTexture{SDL_CreateTexture(
        Core::getRenderer(), pixelFormat, SDL_TEXTUREACCESS_TARGET,
        static_cast<int>(currentTexExtent.w),
        static_cast<int>(currentTexExtent.h))};
    if (rawTexture == nullptr) {
        AUI_LOG_FATAL("Failed to create texture: %s", SDL_GetError());
    }
    currentTexture = std::shared_ptr<SDL_Texture>(
        rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); });

    // Set the new texture as the render target.
    SDL_SetRenderTarget(Core::getRenderer(), currentTexture.get());

    // Tile the image to cover the given extent.
    for (float y{0}; y < currentTexExtent.h; y += sourceHeight) {
        for (float x{0}; x < currentTexExtent.w; x += sourceWidth) {
            // Set up the tile's extents.
            SDL_FRect tileExtent{x, y, sourceWidth, sourceHeight};
            SDL_FRect tileTexExtent{0, 0, sourceWidth, sourceHeight};

            // If the tile is too wide to fit, clip it.
            float tileRight{tileExtent.x + tileExtent.w};
            float offsetRight{currentTexExtent.x + currentTexExtent.w};
            if (tileRight > offsetRight) {
                tileExtent.w -= (tileRight - offsetRight);
                tileTexExtent.w -= (tileRight - offsetRight);
            }

            // If the tile is too tall to fit, clip it.
            float tileBottom{tileExtent.y + tileExtent.h};
            float offsetBottom{currentTexExtent.y + currentTexExtent.h};
            if (tileBottom > offsetBottom) {
                tileExtent.h -= (tileBottom - offsetBottom);
                tileTexExtent.h -= (tileBottom - offsetBottom);
            }

            // Render the tile.
            SDL_RenderTexture(Core::getRenderer(), sourceTexture.get(),
                              &tileTexExtent, &tileExtent);
        }
    }

    // Set the render target back to the window.
    SDL_SetRenderTarget(Core::getRenderer(), nullptr);
}

} // namespace AUI
