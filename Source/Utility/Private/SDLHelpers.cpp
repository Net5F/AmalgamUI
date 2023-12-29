#include "AUI/SDLHelpers.h"
#include "AUI/Internal/Log.h"
#include <algorithm>

namespace AUI
{
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
