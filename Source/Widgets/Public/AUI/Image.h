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
     * @param textureID A user-defined ID (for manually added textures), or the
     *                  full path to an image file.
     * @param scaleMode The filtering/scaling mode that this texture should use 
     *                  ("nearest" by default, to maximize sharpness).
     */
    void setSimpleImage(const std::string& textureID,
                        SDL_ScaleMode scaleMode = SDL_ScaleModeNearest);

    /**
     * Overload to specify texExtent. Use this if you only want to display a
     * portion of the image.
     *
     * @param inTexExtent The extent within the texture to display.
     * @param scaleMode The filtering/scaling mode that this texture should use 
     *                  ("nearest" by default, to maximize sharpness).
     */
    void setSimpleImage(const std::string& textureID, SDL_Rect texExtent,
                        SDL_ScaleMode scaleMode = SDL_ScaleModeNearest);

    /**
     * Sets this widget to render a NineSliceImage (see class comment).
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param textureID A user-defined ID (for manually added textures), or the
     *                  full path to an image file.
     * @param sliceSizes How far to slice into the image, in each direction.
     */
    void setNineSliceImage(const std::string& textureID,
                           NineSliceImage::SliceSizes inSliceSizes);

    struct MultiResImagePathInfo {
        /** The screen resolution that this texture should be used for. */
        ScreenResolution resolution{};
        /** A user-defined ID (for manually added textures), or the full path 
            to an image file. */
        std::string textureID{};
        /** The extent within the texture to display. If left default, the
            full image texture will be used. */
        SDL_Rect texExtent{};
        /** The filtering/scaling mode to use for this texture ("nearest" by 
            default, to maximize sharpness). */
        SDL_ScaleMode scaleMode{SDL_ScaleModeNearest};
    };
    /**
     * Sets this widget to render a MultiResImage (see class comment).
     *
     * Errors if any given path doesn't point to an image file.
     */
    void setMultiResImage(const std::vector<MultiResImagePathInfo>& imageInfo);

    /**
     * Sets this widget to render a TiledImage (see class comment).
     *
     * Errors if the given path doesn't point to an image file.
     *
     * @param textureID A user-defined ID (for manually added textures), or the
     *                  full path to an image file.
     */
    void setTiledImage(const std::string& textureID);

    /**
     * Sets this widget to render the given custom image type.
     */
    void setCustomImage(std::unique_ptr<ImageType> inImageType);

    struct MultiResImageTextureInfo {
        /** The screen resolution that this texture should be used for. */
        ScreenResolution resolution{};
        /** The image texture to take ownership of. */
        SDL_Texture* texture{nullptr};
        /** The ID to associate with the given texture in the asset cache. */
        const std::string& textureID{};
        /** The extent within the texture to display. If left default, the
            full image texture will be used. */
        SDL_Rect texExtent{};
        /** The filtering/scaling mode to use for this texture ("nearest" by 
            default, to maximize sharpness). */
        SDL_ScaleMode scaleMode{SDL_ScaleModeNearest};
    };
    /**
     * Overload that uses the given texture.
     *
     * Note: Ownership of the texture will be taken. Do not free it.
     * Note: These are just for convenience. You can do the same thing by 
     *       calling AUI::Core::getAssetCache().addTexture() before using the 
     *       regular setters.
     *
     * @param texture The image texture to take ownership of.
     * @param textureID The ID to associate with the given texture in the 
     *                  asset cache.
     */
    void setSimpleImage(SDL_Texture* texture, const std::string& textureID,
                        SDL_ScaleMode scaleMode = SDL_ScaleModeNearest);
    void setSimpleImage(SDL_Texture* texture, const std::string& textureID,
                        SDL_Rect texExtent,
                        SDL_ScaleMode scaleMode = SDL_ScaleModeNearest);
    void setNineSliceImage(SDL_Texture* texture, const std::string& textureID,
                           NineSliceImage::SliceSizes inSliceSizes);
    void setMultiResImage(
        const std::vector<MultiResImageTextureInfo>& imageInfo);
    void setTiledImage(SDL_Texture* texture, const std::string& textureID);

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
