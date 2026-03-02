#pragma once

#include "AUI/ImageType/ImageType.h"
#include <string>
#include <memory>

namespace AUI
{
/**
 * Displays an image using the "nine slice" scaling approach to maintain
 * sharpness.
 *
 * The nine slice approach slices the image into 9 pieces (a 3x3 grid).
 * The corners are used as-is. The sides get scaled in a single direction
 * (vertical for the left/right sides, horizontal for the top/bottom. The
 * center gets scaling in both directions.
 *
 * TODO: Add support for tiled borders.
 */
class NineSliceImage : public ImageType
{
public:
    virtual ~NineSliceImage() = default;

    /**
     * Represents how far to slice into the image, in each direction.
     */
    struct SliceSizes {
        float top{0};
        float right{0};
        float bottom{0};
        float left{0};
    };

    /**
     * Sets the image that this widget will slice and render.
     *
     * @param textureID A user-defined ID (for textures added using 
     *                  addTexture()), or the full path to an image file.
     * @param inSliceSizes How far to slice into the image, in each direction.
     * @param scaledExtent The desired size of the generated texture.
     */
    void set(const std::string& textureID, SliceSizes inSliceSizes,
             const SDL_FRect& scaledExtent);

    /**
     * Overridden to generate a new nine slice texture.
     */
    void refresh(const SDL_FRect& scaledExtent) override;

private:
    /**
     * Generates a new nine slice texture, based on sourceTexture.
     */
    void regenerateNineSliceTexture();

    // Functions for copying the slices into the current render target (our
    // new texture). Used by regenerateNineSliceTexture().
    void copyCorners(float sourceWidth, float sourceHeight);
    void copySides(float sourceWidth, float sourceHeight);
    void copyCenter(float sourceWidth, float sourceHeight);

    /** The source texture that we were given. */
    std::shared_ptr<SDL_Texture> sourceTexture;

    /** How far to slice into sourceTexture, in each direction. */
    SliceSizes sliceSizes;
};

} // namespace AUI
