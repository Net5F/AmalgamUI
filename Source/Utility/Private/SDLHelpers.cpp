#include "AUI/SDLHelpers.h"
#include "AUI/Internal/Log.h"
#include <algorithm>

namespace AUI
{
bool SDLHelpers::pointInRect(const SDL_Point& point, const SDL_Rect& rect)
{
    // Test if the point is within all 4 sides of the rect.
    if ((point.x >= rect.x) && (point.x <= (rect.x + rect.w))
        && (point.y >= rect.y) && (point.y <= (rect.y + rect.h))) {
        return true;
    }
    else {
        return false;
    }
}

bool SDLHelpers::rectInRect(const SDL_Rect& rectA, const SDL_Rect& rectB)
{
    // Test if 2 diagonal corners of rectA are within rectB
    SDL_Point topLeft{rectA.x, rectA.y};
    SDL_Point bottomRight{(rectA.x + rectA.w), (rectA.y + rectA.h)};
    return (pointInRect(topLeft, rectB) && pointInRect(bottomRight, rectB));
}

int SDLHelpers::squaredDistance(const SDL_Point& pointA, const SDL_Point& pointB)
{
    // Get the differences between the points.
    int xDif{pointA.x - pointB.x};
    int yDif{pointA.y - pointB.y};

    // Square the differences.
    xDif *= xDif;
    yDif *= yDif;

    // Return the absolute squared distance.
    return std::abs(xDif + yDif);
}

} // namespace AUI
