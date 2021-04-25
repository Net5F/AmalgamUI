#pragma once

#include <SDL_Rect.h>

namespace AUI
{

/**
 * Static functions for scaling between logical and actual sizes.
 */
class ScalingHelpers
{
public:
    /**
     * Scales the given logical extent to its actual screen size, using
     * Core::logicalScreenSize and Core::actualScreenSize.
     */
    static SDL_Rect extentToActual(const SDL_Rect& logicalExtent);

    /**
     * Scales the given actual point to its logical screen position, using
     * Core::logicalScreenSize and Core::actualScreenSize.
     */
    static SDL_Point pointToLogical(const SDL_Point& actualPoint);

    static int fontSizeToActual(int logicalFontSize);
};

} // namespace AUI
