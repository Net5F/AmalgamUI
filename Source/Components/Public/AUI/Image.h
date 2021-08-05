#pragma once

#include "AUI/Component.h"
#include "AUI/ScreenResolution.h"
#include <SDL2/SDL_Render.h>
#include <map>
#include <string>
#include <memory>

namespace AUI
{
/**
 * Displays a simple static image.
 */
class Image : public Component
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Image(Screen& inScreen, const SDL_Rect& inLogicalExtent,
          const std::string& inDebugName = "");

    virtual ~Image() = default;

    /**
     * Adds the given texture to the map of available resolutions.
     *
     * The texture that this component renders will be chosen by comparing
     * Core's current actualScreenSize to the available resolutions.
     *
     * Errors if the given image is nullptr or the given resolution is
     * already in use.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param texture  The texture that this Image should render.
     */
    void addResolution(const ScreenResolution& resolution,
                       const std::shared_ptr<SDL_Texture>& texture);

    /**
     * Overload to specify texExtent. Used if you only want to display a
     * portion of the texture.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param texture  The texture that this Image should render.
     * @param inTexExtent  The extent within the texture to display.
     */
    void addResolution(const ScreenResolution& resolution,
                       const std::shared_ptr<SDL_Texture>& texture,
                       const SDL_Rect& inTexExtent);

    /**
     * Clears this image's current texture and the textures in its
     * resolutionMap.
     */
    void clearTextures();

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void render(const SDL_Point& parentOffset = {}) override;

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
        /** The texture to display. */
        std::shared_ptr<SDL_Texture> texture{};

        /** The extent of the desired image within the texture. */
        SDL_Rect extent{};
    };

    /** Maps screen resolutions to the data that should be used to display this
        image at that resolution.
        Sorted from smallest width to largest. */
    std::map<ScreenResolution, TextureData> resolutionMap;

    /** The current resolution of image to display. */
    std::shared_ptr<SDL_Texture> currentTexture;

    /** The position and size of the desired image within currentTexHandle. */
    SDL_Rect currentTexExtent;
};

} // namespace AUI
