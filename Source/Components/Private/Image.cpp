#include "AUI/Image.h"
#include "AUI/Core.h"
#include <SDL_Render.h>

namespace AUI {

Image::Image(entt::hashed_string key, const SDL_Rect& screenExtent)
: Component(key, screenExtent)
, textureHandle()
, texExtent{}
{
}

void Image::setImage(const std::string& relPath)
{
    // Attempt to load the given texture (errors on failure).
    ResourceManager& resourceManager = Core::GetResourceManager();
    textureHandle = resourceManager.loadTexture(entt::hashed_string(relPath.c_str()));

    // Set the texExtent size to the same as the screenExtent.
    texExtent.w = screenExtent.w;
    texExtent.h = screenExtent.h;
}

void Image::setImage(const std::string& relPath, const SDL_Rect& inTexExtent)
{
    setImage(relPath);

    // Set the texExtent to the given extent.
    texExtent = inTexExtent;
}


void Image::renderCopy(int offsetX, int offsetY)
{
    if (!textureHandle) {
        AUI_LOG_ERROR("Tried to render Image with no texture. Key: %s", key.data());
    }

    // Account for the given offset.
    SDL_Rect offsetTex{texExtent};
    offsetTex.x += offsetX;
    offsetTex.y += offsetY;

    SDL_Rect offsetScreen{screenExtent};
    offsetScreen.x += offsetX;
    offsetScreen.y += offsetY;

    // Render the image.
    SDL_RenderCopy(Core::GetRenderer(), &(*textureHandle), &offsetTex, &offsetScreen);
}

} // namespace AUI
