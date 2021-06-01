#pragma once

#include <SDL_Rect.h>

namespace AUI
{

/**
 * Static functions for scaling between logical and actual sizes.
 *
 * Note: UI components exist in two spaces: logical and actual.
 *       Logical screen space is used for all developer-given component
 *       measurements, and is relative to the value of Core::logicalScreenSize.
 *       Actual screen space is what's actually used in rendering to the
 *       screen, and is relative to the value of Core::actualScreenSize.
 *
 *       The intent is that the developer will set a logical screen size and
 *       give all width, height, position, etc values in reference to that
 *       logical size. Then, the user can select a new actual size and the UI
 *       will scale to it intelligently.
 */
class ScalingHelpers
{
public:
    /**
     * Scales the given logical extent to match the current UI scaling,
     * returning its actual-space equivalent.
     * Uses Core::logicalScreenSize and Core::actualScreenSize.
     */
    static SDL_Rect extentToActual(const SDL_Rect& logicalExtent);

    /**
     * Scales the given logical point to match the current UI scaling,
     * returning its actual-space equivalent.
     * Uses Core::logicalScreenSize and Core::actualScreenSize.
     */
    static SDL_Point pointToActual(const SDL_Point& logicalPoint);

    /**
     * Applies the inverse of the current UI scaling to the given point,
     * returning its logical screen position.
     * Uses Core::logicalScreenSize and Core::actualScreenSize.
     */
    static SDL_Point pointToLogical(const SDL_Point& actualPoint);

    static int fontSizeToActual(int logicalFontSize);
};

} // namespace AUI
