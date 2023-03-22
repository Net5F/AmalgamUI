#pragma once

#include "AUI/Widget.h"
#include "AUI/Internal/Ignore.h"
#include <SDL_render.h>
#include <string>
#include <memory>

namespace AUI
{
struct ImageType {
public:
    /**
     * Call this when the UI scale changes, so the derived type can regenerate
     * the image if necessary.
     *
     * @param scaledExtent  The extent of the parent Image widget. Some image
     *                      types use this in their render logic.
     */
    virtual void refresh(const SDL_Rect& scaledExtent);

protected:
    // Friend class so Image::render() can use these fields, but outside users
    // have to go through a derived class's setter.
    friend class Image;

    /** The current texture to display. */
    std::shared_ptr<SDL_Texture> currentTexture{};

    /** The extent within currentTexture to display. */
    SDL_Rect currentTexExtent{};
};

} // namespace AUI
