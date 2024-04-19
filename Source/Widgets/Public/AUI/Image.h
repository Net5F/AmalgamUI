#pragma once

#include "AUI/Widget.h"
#include "AUI/ScreenResolution.h"
#include "AUI/ImageType/ImageType.h"
#include "AUI/ImageType/NineSliceImage.h"
#include <SDL_render.h>
#include <string>
#include <memory>

namespace AUI
{
/**
 * Displays an image.
 *
 * To use:
 *   1. Construct this widget.
 *   2. Use one of the "setXYZ" convenience functions to set the image to
 *      one of the built-in image types.
 *      Or, use setCustomImage() to set your own custom image type.
 *
 * Built-in image types:
 *   SimpleImage
 *     Simply renders the image, stretching or squashing as needed.
 *   NineSliceImage
 *     Slices the image into 9 pieces, preserving the corners and stretching
 *     the sides and center in a way that maintains image sharpness.
 *   MultiResImage
 *     Allows you to add multiple resolutions of an image, which will be
 *     selected between based on the current screen resolution.
 *   TiledImage
 *     Tiles the image.
 *
 * Note: Image assets are managed in the AssetCache class.
 */
class Image : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Image(const SDL_Rect& inLogicalExtent,
          const std::string& inDebugName = "Image");

    virtual ~Image() = default;

    /**
     * Sets this widget to render a SimpleImage (see class comment).
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param imagePath  The full path to the image file.
     */
    void setSimpleImage(const std::string& imagePath);

    /**
     * Overload to specify texExtent. Use this if you only want to display a
     * portion of the image.
     *
     * @param inTexExtent  The extent within the texture to display.
     */
    void setSimpleImage(const std::string& imagePath, SDL_Rect texExtent);

    /**
     * Sets this widget to render a NineSliceImage (see class comment).
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param imagePath  The full path to the image file.
     * @param sliceSizes  How far to slice into the image, in each direction.
     */
    void setNineSliceImage(const std::string& imagePath,
                           NineSliceImage::SliceSizes inSliceSizes);

    struct MultiResImageInfo {
        /** The screen resolution that this texture should be used for. */
        ScreenResolution resolution{};
        /** The full path to the image file. */
        std::string imagePath{};
        /** The extent within the texture to display. If left default, the
            full image texture will be used. */
        SDL_Rect texExtent{};
    };
    /**
     * Sets this widget to render a MultiResImage (see class comment).
     *
     * Errors if any given path doesn't point to an image file.
     */
    void setMultiResImage(const std::vector<MultiResImageInfo>& imageInfo);

    /**
     * Sets this widget to render a TiledImage (see class comment).
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param imagePath  The full path to the image file.
     */
    void setTiledImage(const std::string& imagePath);

    /**
     * Sets this widget to render the given custom image type.
     */
    void setCustomImage(std::unique_ptr<ImageType> inImageType);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void measure(const SDL_Rect& availableExtent);

    /**
     * Calls Widget::arrange() and refreshes the image if this widget's
     * scaledExtent changed.
     */
    void arrange(const SDL_Point& startPosition,
                 const SDL_Rect& availableExtent,
                 WidgetLocator* widgetLocator) override;

    void render(const SDL_Point& windowTopLeft) override;

private:
    std::unique_ptr<ImageType> imageType;

    /** Holds the scaled extent that was used during the last updateLayout().
        Used to tell when this widget's size changes, so we can refresh the
        image. */
    SDL_Rect lastScaledExtent;
};

} // namespace AUI
