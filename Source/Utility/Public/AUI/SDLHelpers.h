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
     * Returns true if the given rect contains the given point.
     *
     * Note: If the point is exactly on the edge of the rect, it still counts
     *       as being inside.
     *
     * A replacement for SDL_PointInRect(), which for some reason is not
     * inclusive of points on the far edges.
     */
    static bool pointInRect(const SDL_Point& point, const SDL_Rect& rect);

    /**
     * Returns true if rectB fully contains rectA.
     *
     * Note: If part of rectA is exactly on the edge of rectB, that part still
     *       counts as being inside.
     */
    static bool rectInRect(const SDL_Rect& rectA, const SDL_Rect& rectB);

    /**
     * Returns the squared distance between the given points.
     */
    static int squaredDistance(const SDL_Point& pointA, const SDL_Point& pointB);
};

} // namespace AUI
