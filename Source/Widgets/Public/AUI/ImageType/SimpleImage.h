#pragma once

#include "AUI/ImageType/ImageType.h"
#include <string>

namespace AUI
{
/**
 * Displays a simple static image.
 *
 * If the render extent is larger or smaller than the source image, the image
 * will be squashed or stretched.
 */
class SimpleImage : public ImageType
{
public:
    virtual ~SimpleImage() = default;

    /**
     * Sets the image that this widget will render.
     *
     * @param textureID A user-defined ID (for manually added textures), or the
     *                  full path to an image file.
     */
    void set(const std::string& textureID);

    /**
     * Overload to specify texExtent. Use this if you only want to display a
     * portion of the texture.
     *
     * @param inTexExtent The extent within the texture to display.
     */
    void set(const std::string& textureID, const SDL_Rect& inTexExtent);

    /**
     * Clears this image's current texture.
     */
    void clear();
};

} // namespace AUI
