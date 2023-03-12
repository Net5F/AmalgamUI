#pragma once

#include "AUI/Widget.h"
#include <SDL_render.h>
#include <map>
#include <string>
#include <memory>

namespace AUI
{
/**
 * Displays an image, using the "nine slice" approach to maintain sharpness.
 */
class NineSliceImage : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    NineSliceImage(const SDL_Rect& inLogicalExtent,
          const std::string& inDebugName = "NineSliceImage");

    virtual ~NineSliceImage() = default;

    void setTexture(const std::shared_ptr<SDL_Texture>& texture);

    //-------------------------------------------------------------------------
    // Image class overrides
    //-------------------------------------------------------------------------
    virtual void addResolution(const ScreenResolution& resolution,
                               const std::shared_ptr<SDL_Texture>& texture);
    virtual void addResolution(const ScreenResolution& resolution,
                               const std::shared_ptr<SDL_Texture>& texture,
                               const SDL_Rect& inTexExtent);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void render() override;

protected:
    /**
     * Overridden to generate new nine slice texture.
     */
    bool refreshScaling() override;

    /** The original texture that we were given. */
    std::shared_ptr<SDL_Texture> originalTexture;

    /** The generated nine slice texture. */
    std::shared_ptr<SDL_Texture> nineSliceTexture;
};

} // namespace AUI
