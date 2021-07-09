#include "AUI/TiledImage.h"
#include "AUI/Core.h"
#include <SDL_Render.h>

namespace AUI {

TiledImage::TiledImage(Screen& inScreen, const char* key, const SDL_Rect& logicalExtent)
: Image(inScreen, key, logicalExtent)
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

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // If we don't have a texture to render, fail.
    if (!currentTexHandle) {
        AUI_LOG_ERROR("Tried to render Image with no texture. Key: %s", key.data());
    }

    // Tile the image to cover this component's extent.
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
            SDL_RenderCopy(Core::GetRenderer(), &(*currentTexHandle)
                , &tileTexExtent, &tileExtent);
        }
    }
}

} // namespace AUI
