#include "AUI/SDLHelpers.h"
#include "AUI/Internal/Log.h"
#include <algorithm>

namespace AUI
{
float SDLHelpers::squaredDistance(const SDL_FPoint& pointA,
                                  const SDL_FPoint& pointB)
{
    // Get the differences between the points.
    float xDif{pointA.x - pointB.x};
    float yDif{pointA.y - pointB.y};

    // Square the differences.
    xDif *= xDif;
    yDif *= yDif;

    // Return the absolute squared distance.
    return std::abs(xDif + yDif);
}

bool SDLHelpers::hasPositiveArea(const SDL_FRect& rect)
{
    return (rect.w > 0.0f) && (rect.h > 0.0f);
}

} // namespace AUI
