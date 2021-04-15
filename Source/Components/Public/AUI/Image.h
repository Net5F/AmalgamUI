#pragma once

#include "AUI/Component.h"
#include "AUI/Internal/ResourceManager.h" // TextureHandle

namespace AUI {

/**
 * Displays a simple static image.
 */
class Image : public Component
{
public:
    Image(Screen& screen, const char* key, const SDL_Rect& screenExtent);

    virtual ~Image() = default;

    /**
     * Sets this component's image to the one at the given path.
     *
     * Errors if the given path is not a valid image.
     *
     * Gets the texture at Core::resourcePath + relPath from the resource
     * manager, and sets texExtent equal to screenExtent.
     */
    void setImage(const std::string& relPath);

    /**
     * Overload to specify texExtent. Used if you only want to display a
     * portion of the texture.
     *
     * @param inTexExtent  The extent within the texture to display.
     */
    void setImage(const std::string& relPath, const SDL_Rect& inTexExtent);

    void render(int offsetX = 0, int offsetY = 0) override;

private:
    /** The texture containing the image to display. */
    TextureHandle textureHandle;

    /** The position and size of the desired image within the texture. */
    SDL_Rect texExtent;
};

} // namespace AUI
