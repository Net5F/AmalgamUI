#pragma once

#include "AUI/Image.h"

namespace AUI
{
/**
 * Tiles an image across this widget's extent.
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
               const std::string& inDebugName = "TiledImage");

    virtual ~TiledImage() = default;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void render() override;
};

} // namespace AUI
