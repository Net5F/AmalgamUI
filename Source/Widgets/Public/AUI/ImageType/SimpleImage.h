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
    /**
     * Sets the image that this widget will render.
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param imagePath  The full path to the image file.
     */
    void set(const std::string& imagePath);

    /**
     * Overload to specify texExtent. Use this if you only want to display a
     * portion of the texture.
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param imagePath  The full path to the image file.
     * @param inTexExtent  The extent within the texture to display.
     */
    void set(const std::string& imagePath, const SDL_Rect& inTexExtent);

    /**
     * Clears this image's current texture.
     */
    void clear();
};

} // namespace AUI
