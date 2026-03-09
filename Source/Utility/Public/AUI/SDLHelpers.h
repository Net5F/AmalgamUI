#pragma once

#include <SDL3/SDL_rect.h>

namespace AUI
{
/**
 * Static functions for working with SDL types.
 */
class SDLHelpers
{
public:
    /**
     * Returns the squared distance between the given points.
     */
    static float squaredDistance(const SDL_FPoint& pointA,
                                 const SDL_FPoint& pointB);

};

} // namespace AUI
