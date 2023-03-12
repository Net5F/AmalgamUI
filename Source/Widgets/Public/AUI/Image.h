#pragma once

#include "AUI/Widget.h"
#include "AUI/AssetCache.h" // TextureHandle
#include "AUI/ScreenResolution.h"
#include <SDL_render.h>
#include <map>
#include <string>
#include <memory>

namespace AUI
{
/**
 * Displays a simple static image.
 *
 * To use:
 *   1. Add one or more resolutions of your image using addResolution().
 *   2. Set your screen size through Core::setActualScreenSize().
 *
 *   This Image will render the added image texture that matches the actual
 *   screen size. If no added resolution matches, the largest one will be used
 *   (for the best chance at looking nice after scaling).
 *
 * Note: Image assets are managed in an internal cache.
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
     * Adds the given texture to the map of available resolutions.
     *
     * The texture that this widget renders will be chosen by comparing
     * Core's current actualScreenSize to the available resolutions.
     *
     * Errors if the given image is nullptr or the given resolution is
     * already in use.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param relPath  The path to the image to add, relative to
     *                 Core::resourcePath.
     */
    virtual void addResolution(const ScreenResolution& resolution,
                               const std::string& relPath);

    /**
     * Overload to specify texExtent. Used if you only want to display a
     * portion of the texture.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param relPath  The path to the image to add, relative to
     *                 Core::resourcePath.
     * @param inTexExtent  The extent within the texture to display.
     */
    virtual void addResolution(const ScreenResolution& resolution,
                               const std::string& relPath,
                               const SDL_Rect& inTexExtent);

    /**
     * Clears this image's current texture and the textures in its
     * resolutionMap.
     */
    void clearTextures();

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void render() override;

protected:
    /**
     * Overridden to choose the proper resolution of texture to use.
     */
    bool refreshScaling() override;

    /**
     * Re-calculates which resolution of texture to use, based on Core's
     * current actualScreenSize and the available resolutions in resolutionMap.
     */
    void refreshChosenResolution();

    /**
     * The data needed to render an image's texture.
     */
    struct TextureData {
        /** The relative path to the image file. */
        std::string relPath;

        /** If true, the user gave us an extent to use. If false, we'll 
            use the full texture. */
        bool userProvidedExtent{false};

        /** If userProvidedExtent is true, holds the extent of the desired 
            image within the texture. */
        SDL_Rect extent{};
    };

    /** Maps screen resolutions to the data that should be used to display this
        image at that resolution. */
    std::map<ScreenResolution, TextureData> resolutionMap;

    /** The current resolution of image to display. */
    std::shared_ptr<SDL_Texture> currentTexture;

    /** The position and size of the desired image within currentTexHandle. */
    SDL_Rect currentTexExtent;
};

} // namespace AUI
