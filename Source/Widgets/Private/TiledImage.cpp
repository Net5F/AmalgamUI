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

void TiledImage::render()
{
    // If we don't have a texture to render, fail.
    if (!currentTexture) {
        AUI_LOG_FATAL("Tried to render Image with no texture. DebugName: %s",
                      debugName.c_str());
    }

    // Tile the image to cover this widget's extent.
    for (int y = renderExtent.y; y < (renderExtent.y + renderExtent.h);
         y += currentTexExtent.h) {
        for (int x = renderExtent.x; x < (renderExtent.x + renderExtent.w);
             x += currentTexExtent.w) {
            // Set up the tile's extents.
            SDL_Rect tileExtent{x, y, currentTexExtent.w, currentTexExtent.h};
            SDL_Rect tileTexExtent{currentTexExtent};

            // If the tile is too wide to fit, clip it.
            int tileRight{tileExtent.x + tileExtent.w};
            int offsetRight{renderExtent.x + renderExtent.w};
            if (tileRight > offsetRight) {
                tileExtent.w -= (tileRight - offsetRight);
                tileTexExtent.w -= (tileRight - offsetRight);
            }

            // If the tile is too tall to fit, clip it.
            int tileBottom{tileExtent.y + tileExtent.h};
            int offsetBottom{renderExtent.y + renderExtent.h};
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
