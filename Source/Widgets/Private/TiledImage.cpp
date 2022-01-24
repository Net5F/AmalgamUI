#include "AUI/TiledImage.h"
#include "AUI/Core.h"
#include <SDL2/SDL_render.h>

namespace AUI
{
TiledImage::TiledImage(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                       const std::string& inDebugName)
: Image(inScreen, inLogicalExtent, inDebugName)
{
}

void TiledImage::render(const SDL_Point& parentOffset)
{
    // Keep our scaling up to date.
    refreshScaling();

    // Account for the given offset.
    SDL_Rect offsetExtent{scaledExtent};
    offsetExtent.x += parentOffset.x;
    offsetExtent.y += parentOffset.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = offsetExtent;

    // If the widget isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // If we don't have a texture to render, fail.
    if (!currentTexture) {
        AUI_LOG_FATAL("Tried to render Image with no texture. DebugName: %s",
                      debugName.c_str());
    }

    // Tile the image to cover this widget's extent.
    for (int y = offsetExtent.y; y < (offsetExtent.y + offsetExtent.h);
         y += currentTexExtent.h) {
        for (int x = offsetExtent.x; x < (offsetExtent.x + offsetExtent.w);
             x += currentTexExtent.w) {
            // Set up the tile's extents.
            SDL_Rect tileExtent{x, y, currentTexExtent.w, currentTexExtent.h};
            SDL_Rect tileTexExtent{currentTexExtent};

            // If the tile is too wide to fit, clip it.
            int tileRight{tileExtent.x + tileExtent.w};
            int offsetRight{offsetExtent.x + offsetExtent.w};
            if (tileRight > offsetRight) {
                tileExtent.w -= (tileRight - offsetRight);
                tileTexExtent.w -= (tileRight - offsetRight);
            }

            // If the tile is too tall to fit, clip it.
            int tileBottom{tileExtent.y + tileExtent.h};
            int offsetBottom{offsetExtent.y + offsetExtent.h};
            if (tileBottom > offsetBottom) {
                tileExtent.h -= (tileBottom - offsetBottom);
                tileTexExtent.h -= (tileBottom - offsetBottom);
            }

            // Render the tile.
            SDL_RenderCopy(Core::getRenderer(), currentTexture.get(),
                           &tileTexExtent, &tileExtent);
        }
    }
}

} // namespace AUI