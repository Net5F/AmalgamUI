#pragma once

#include "AUI/Component.h"
#include "AUI/Internal/ResourceManager.h" // TextureHandle

namespace AUI {

class AmalgamUI;

/**
 * Displays a simple static image.
 */
class Image : public Component
{
public:
    Image(entt::hashed_string key, const SDL_Rect& screenExtent);

    virtual ~Image() = default;

    /**
     * Gets the texture at Core::resourcePath + relPath from the resource
     * manager, and sets texExtent equal to screenExtent.
     */
    void setImage(const std::string& relPath);

    /**
     * Overload to specify texExtent. Used if the component's screen extent
     * isn't equal to the texture extent.
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
