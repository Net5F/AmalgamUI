#pragma once

#include "AUI/ImageType/ImageType.h"
#include "AUI/ScreenResolution.h"
#include <SDL_render.h>
#include <map>
#include <string>
#include <memory>

namespace AUI
{
/**
 * Allows you to add multiple resolutions of an image, which will then be
 * selected between based on the current screen resolution.
 *
 * To use:
 *   1. Add one or more resolutions of your image using addResolution().
 *   2. Set your screen size through Core::setActualScreenSize().
 *
 *   This widget will render the added image texture that matches the actual
 *   screen size. If no added resolution matches, the largest one will be used
 *   (for the best chance at looking nice after scaling).
 */
class MultiResImage : public ImageType
{
public:
    /**
     * Adds the given image to the map of available resolutions.
     *
     * The texture that this widget renders will be chosen by comparing
     * Core's current actualScreenSize to the available resolutions.
     *
     * Errors if the given path doesn't point to an image file, or the given
     * resolution is already in use.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param imagePath  The full path to the image file.
     */
    void addResolution(const ScreenResolution& resolution,
                       const std::string& imagePath);

    /**
     * Overload to specify texExtent. Used if you only want to display a
     * portion of the texture.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param imagePath  The full path to the image file.
     * @param inTexExtent  The extent within the texture to display.
     */
    void addResolution(const ScreenResolution& resolution,
                       const std::string& imagePath, const SDL_Rect& texExtent);

    /**
     * Clears this image's current texture and the textures in its
     * resolutionMap.
     */
    void clear();

    /**
     * Overridden to choose the proper resolution of texture to use.
     */
    void refresh(const SDL_Rect& scaledExtent) override;

private:
    /**
     * Re-calculates which resolution of texture to use, based on Core's
     * current actualScreenSize and the available resolutions in resolutionMap.
     */
    void refreshChosenResolution();

    /**
     * The data needed to render an image's texture.
     */
    struct TextureData {
        /** The full path to the image file. */
        std::string imagePath;

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
};

} // namespace AUI
