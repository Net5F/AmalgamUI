#include "AUI/ImageType/TiledImage.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"
#include <SDL_render.h>

namespace AUI
{
void TiledImage::set(const std::string& imagePath, const SDL_Rect& scaledExtent)
{
    // Attempt to load the image (errors on failure).
    AssetCache& assetCache{Core::getAssetCache()};
    sourceTexture = assetCache.requestTexture(imagePath);

    // We're going to generate a texture as large as the given extent.
    currentTexExtent.w = scaledExtent.w;
    currentTexExtent.h = scaledExtent.h;

    // Re-generate our tiled texture.
    regenerateTiledTexture();
}

void TiledImage::refresh(const SDL_Rect& scaledExtent)
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
    Uint32 pixelFormat{};
    int sourceWidth{};
    int sourceHeight{};
    SDL_QueryTexture(sourceTexture.get(), &pixelFormat, nullptr, &sourceWidth,
                     &sourceHeight);

    // Allocate the new texture.
    SDL_Texture* rawTexture{SDL_CreateTexture(
        Core::getRenderer(), pixelFormat, SDL_TEXTUREACCESS_TARGET,
        currentTexExtent.w, currentTexExtent.h)};
    if (rawTexture == nullptr) {
        AUI_LOG_FATAL("Failed to create texture: %s", SDL_GetError());
    }
    currentTexture = std::shared_ptr<SDL_Texture>(
        rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); });

    // Set the new texture as the render target.
    SDL_SetRenderTarget(Core::getRenderer(), currentTexture.get());

    // Tile the image to cover the given extent.
    for (int y = 0; y < currentTexExtent.h; y += sourceHeight) {
        for (int x = 0; x < currentTexExtent.w; x += sourceWidth) {
            // Set up the tile's extents.
            SDL_Rect tileExtent{x, y, sourceWidth, sourceHeight};
            SDL_Rect tileTexExtent{0, 0, sourceWidth, sourceHeight};

            // If the tile is too wide to fit, clip it.
            int tileRight{tileExtent.x + tileExtent.w};
            int offsetRight{currentTexExtent.x + currentTexExtent.w};
            if (tileRight > offsetRight) {
                tileExtent.w -= (tileRight - offsetRight);
                tileTexExtent.w -= (tileRight - offsetRight);
            }

            // If the tile is too tall to fit, clip it.
            int tileBottom{tileExtent.y + tileExtent.h};
            int offsetBottom{currentTexExtent.y + currentTexExtent.h};
            if (tileBottom > offsetBottom) {
                tileExtent.h -= (tileBottom - offsetBottom);
                tileTexExtent.h -= (tileBottom - offsetBottom);
            }

            // Render the tile.
            SDL_RenderCopy(Core::getRenderer(), sourceTexture.get(),
                           &tileTexExtent, &tileExtent);
        }
    }

    // Set the render target back to the window.
    SDL_SetRenderTarget(Core::getRenderer(), nullptr);
}

} // namespace AUI
