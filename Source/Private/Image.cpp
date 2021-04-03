#include "AUI/Image.h"
#include <SDL_Render.h>

namespace AUI {

void Image::Image(int inX, int inY, int inWidth, int inHeight, const std::shared_ptr<SDL_Texture>& inTexture)
: x{inX}
, y{inY}
, width{inWidth}
, height{inHeight}
, texture(inTexture)
{
}

void Image::renderCopy(const SDL_Renderer* renderer)
{
    SDL_Rect extent{x, y, width, height};
    SDL_RenderCopy(renderer, texture, extent, extent);
}

} // namespace AUI
