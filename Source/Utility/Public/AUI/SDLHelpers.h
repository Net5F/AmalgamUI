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

    /**
     * Returns true if the given rect has a positive area.
     *
     * Often used to check if a rect has been fully clipped.
     * 
     * Note: We can't use SDL_GetRectEmptyFloat because it considers 0-sized 
     *       rects to be non-empty.
     */
    static bool hasPositiveArea(const SDL_FRect& rect);
};

} // namespace AUI
