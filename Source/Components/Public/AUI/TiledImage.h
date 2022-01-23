#pragma once

#include "AUI/Image.h"

namespace AUI
{
/**
 * Tiles an image across this component's extent.
 *
 * If the given image doesn't tile perfectly, it will be clipped at the right
 * and bottom edges.
 */
class TiledImage : public Image
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    TiledImage(Screen& inScreen, const SDL_Rect& inLogicalExtent,
               const std::string& inDebugName = "");

    virtual ~TiledImage() = default;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void render(const SDL_Point& parentOffset = {}) override;
};

} // namespace AUI
