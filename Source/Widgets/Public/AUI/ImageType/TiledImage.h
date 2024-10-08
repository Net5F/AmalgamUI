#pragma once

#include "AUI/ImageType/ImageType.h"
#include <memory>

namespace AUI
{
/**
 * Tiles an image across this widget's extent.
 *
 * If the given image doesn't tile perfectly, it will be clipped at the right
 * and bottom edges.
 */
class TiledImage : public ImageType
{
public:
    virtual ~TiledImage() = default;

    /**
     * Sets the image that this widget will tile and render.
     *
     * @param textureID A user-defined ID (for manually added textures), or the
     *                  full path to an image file.
     * @param scaledExtent The desired size of the generated texture.
     */
    void set(const std::string& textureID, const SDL_Rect& scaledExtent);

    /**
     * Overridden to generate a new tiled image texture.
     */
    void refresh(const SDL_Rect& scaledExtent) override;

private:
    /**
     * Generates a new tiled texture, based on sourceTexture.
     */
    void regenerateTiledTexture();

    /** The source texture that we were given. */
    std::shared_ptr<SDL_Texture> sourceTexture;
};

} // namespace AUI
