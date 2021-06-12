#pragma once

#include "AUI/Component.h"
#include "AUI/ScreenResolution.h"
#include "AUI/ResourceManager.h" // TextureHandle
#include <map>

namespace AUI {

/**
 * Displays a simple static image.
 */
class Image : public Component
{
public:
    Image(Screen& screen, const char* key, const SDL_Rect& logicalExtent);

    virtual ~Image() = default;

    /**
     * Adds the given texture to the map of available resolutions.
     *
     * The texture that this component renders will be chosen by comparing
     * Core's current actualScreenSize to the available resolutions.
     *
     * Errors if the given path is not a valid image or the given resolution
     * is already in use.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param relPath  The path relative to Core::resourcePath where the
     *                 texture resides.
     */
    void addResolution(const ScreenResolution& resolution, const std::string& relPath);

    /**
     * Overload to specify texExtent. Used if you only want to display a
     * portion of the texture.
     *
     * @param resolution  The actual screen resolution that this texture
     *                    should be used for.
     * @param relPath  The path relative to Core::resourcePath where the
     *                 texture resides.
     * @param inTexExtent  The extent within the texture to display.
     */
    void addResolution(const ScreenResolution& resolution, const std::string& relPath, const SDL_Rect& inTexExtent);

    void render(const SDL_Point& parentOffset = {}) override;

protected:
    /**
     * Overridden to choose the proper resolution of texture to use.
     */
    bool refreshScaling() override;

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
        /** The texture to display. */
        TextureHandle handle{};

        /** The extent of the desired image within the texture. */
        SDL_Rect extent{};
    };

    /** Maps screen resolutions to the data that should be used to display this
        image at that resolution.
        Sorted from smallest width to largest. */
    std::map<ScreenResolution, TextureData> resolutionMap;

    /** The current resolution of image to display. */
    TextureHandle currentTexHandle;

    /** The position and size of the desired image within currentTexHandle. */
    SDL_Rect currentTexExtent;
};

} // namespace AUI
