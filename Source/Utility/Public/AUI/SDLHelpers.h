#pragma once

#include <SDL_rect.h>

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
    static int squaredDistance(const SDL_Point& pointA, const SDL_Point& pointB);
};

} // namespace AUI
