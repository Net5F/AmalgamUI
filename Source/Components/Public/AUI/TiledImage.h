#pragma once

#include "AUI/Image.h"

namespace AUI {

/**
 * Tiles an image across this component's extent.
 *
 * If the given image doesn't tile perfectly, it will be clipped at the left
 * and bottom edges.
 */
class TiledImage : public Image
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    TiledImage(Screen& inScreen, const char* key, const SDL_Rect& logicalExtent);

    virtual ~TiledImage() = default;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void render(const SDL_Point& parentOffset = {}) override;
};

} // namespace AUI
